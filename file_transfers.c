#include "main.h"

//static FILE_TRANSFER *file_t[256], **file_tend = file_t;
static FILE_TRANSFER active_transfer[MAX_NUM_FRIENDS][MAX_FILE_TRANSFERS];

void file_transfer_local_control(Tox *tox, uint32_t friend_number, uint32_t file_number, TOX_FILE_CONTROL control){
    TOX_ERR_FILE_CONTROL error = 0;

    switch(control){
        case TOX_FILE_CONTROL_RESUME:
            // if not started
            debug("FileTransfer:\tWe just accepted file (%i)\n", friend_number, file_number);
            // else
            debug("FileTransfer:\tWe just resumed file (%i)\n", friend_number, file_number);
            tox_file_send_control(tox, friend_number, file_number, control, &error);
            break;
        case TOX_FILE_CONTROL_PAUSE:
            debug("FileTransfer:\tWe just paused file (%i)\n", friend_number, file_number);
            tox_file_send_control(tox, friend_number, file_number, control, &error);
            break;
        case TOX_FILE_CONTROL_CANCEL:
            debug("FileTransfer:\tWe just canceled file (%i)\n", friend_number, file_number);
            tox_file_send_control(tox, friend_number, file_number, control, &error);
            break;
    }
    if(error){
        debug("FileTransfer:\tThere was an error sending the command, you probably want to see to that!\n");
    }
}

static void file_transfer_callback_control(Tox *tox, uint32_t friend_number, uint32_t file_number,
                                                                    TOX_FILE_CONTROL control, void *UNUSED(userdata)){
    switch(control){
        case TOX_FILE_CONTROL_RESUME:
            // if not started
            debug("FileTransfer:\tFriend (%i) has accepted file (%i)\n", friend_number, file_number);
            // else
            debug("FileTransfer:\tFriend (%i) has resumed file (%i)\n", friend_number, file_number);

            break;
        case TOX_FILE_CONTROL_PAUSE:
            debug("FileTransfer:\tFriend (%i) has paused file (%i)\n", friend_number, file_number);
            break;
        case TOX_FILE_CONTROL_CANCEL:
            debug("FileTransfer:\tFriend (%i) has canceled file (%i)\n", friend_number, file_number);
            break;
    }
}

/* Function called by core with a new incoming file. */
static void incoming_file_callback_request(Tox *tox, uint32_t friend_number, uint32_t file_number, uint32_t kind,
                                 uint64_t file_size, const uint8_t *filename, size_t filename_length, void *user_data){
    //new incoming file
    debug("FileTransfer:\tNew incoming file from friend(%i)\nFileTransfer:\t\tfilename: %s\n", friend_number, filename);

    TOX_ERR_FILE_CONTROL error;
    FILE_TRANSFER *file_handle = &active_transfer[friend_number][file_number];

    /* if(file_handle->status > 0 ){
        tox_file_send_control(tox, friend_number, file_number, TOX_FILE_CONTROL_CANCEL, &error);
        debug("FileTransfer:\tNew incoming file with existing id. Friend(%i)&File(%u).\n", friend_number, file_number);
        // TODO destroy and reset this file, it has become corrupt across clients.
        memset(file_handle, 0, sizeof(FILE_T));
        return;
    }*/

    // Reset the file handle for new data.
    memset(file_handle, 0, sizeof(FILE_TRANSFER));

    // Set ids
    file_handle->friend = friend_number;
    file_handle->file = file_number;
    file_handle->size = file_size;

    // FILE_T->filename_length is our max length, make sure that's enforced!
    filename_length = filename_length > sizeof(file_handle->name) ? sizeof(file_handle->name) : filename_length;
    filename_length = utf8_validate(filename, filename_length);
    file_handle->name_length = filename_length;
    memcpy(file_handle->name, filename, filename_length);

    // If it's a small inline image, just accept it!
    if( file_size < 1024 * 1024 * 4 && filename_length == sizeof("utox-inline.png") - 1 &&
                                memcmp(filename, "utox-inline.png", filename_length) == 0) {
        file_handle->in_memory = 1;
        file_handle->status = FILE_TRANSFER_STATUS_ACTIVE;
        file_handle->data = malloc(file_size);
        tox_file_send_control(tox, friend_number, file_number, TOX_FILE_CONTROL_RESUME, &error);
        postmessage(FRIEND_FILE_IN_NEW_INLINE, friend_number, file_number, NULL);
    } else {
        postmessage(FRIEND_FILE_IN_NEW, friend_number, file_number, NULL);
    }
}

static void incoming_file_callback_chunk(Tox *tox, uint32_t friend_number, uint32_t file_number, uint64_t position,
                                                                const uint8_t *data, size_t length, void *user_data){
    debug("FileTransfer:\tIncoming chunk for friend (%u), and file (%u). Start (%u), End (%u).\r",
                                                                      friend_number, file_number, position, length);
    if(length == 0){
        debug("\nFileTransfer:\tIncoming transfer is done (%u & %u)\n", friend_number, file_number);
        return;
    }

    TOX_ERR_FILE_SEND_CHUNK error;
    FILE_TRANSFER *file_handle = &active_transfer[friend_number][file_number];
    FILE *file = file_handle->data;
    uint64_t last_bit = position + length;
    // time_t time = time(NULL);
        file_handle->last_chunk_time = time(NULL);

    if(file_handle->in_memory) {
        memcpy(file_handle->data + file_handle->size_received, data, length);
    } else {
        fseeko(file, 0, position);
        size_t write_size = fwrite(data, 1, length, file);
        if(write_size != length){
            debug("\n\nFileTransfer:\tERROR WRITING DATA TO FILE! (%u & %u)\n\n", friend_number, file_number);
            tox_postmessage(TOX_FILE_INCOMING_CANCEL, friend_number, file_number, NULL);
            return;
        }
    }
    file_handle->size_received += length;

    if(time(NULL) - file_handle->last_chunk_time >= 1) {
    // TODO divide total size by total time to get ave speed.
    // include last active, and total paused time to exclude paused time.
        FILE_PROGRESS *p = malloc(sizeof(FILE_PROGRESS));
        p->size_received = file_handle->size_received;
        p->speed = ( (file_handle->size_received - file_handle->last_chunk_time) )
                        / (time(NULL) - file_handle->last_chunk_time);
        postmessage(FRIEND_FILE_IN_PROGRESS, friend_number, file_number, p);
        file_handle->size_received = file_handle->size_received;
    }

}

void outgoing_file_send_new(Tox *tox, uint32_t friend_number, uint8_t *path, const uint8_t *filename, size_t filename_length){

    debug("FileTransfer:\tStarting outgoing file to friend %u. (filename, %s)\n", friend_number, filename);

    //     FILE_TRANSFER *file_handle = active_transfer[friend_number][file_number]; TODO
    if(friend[friend_number].transfer_count >= MAX_FILE_TRANSFERS) {
        debug("FileTransfer:\tMaximum outgoing file sending limit reached(%u/%u) for friend(%u). ABORTING!\n",
                                            friend[friend_number].transfer_count, MAX_FILE_TRANSFERS, friend_number);
        return;
    }

    FILE *file = fopen((char*)path, "rb");
    if(!file) {
        debug("FileTransfer:\tUnable to open file for reading!\n");
        return;
    }


    TOX_ERR_FILE_SEND error;
    uint64_t file_size = 0;
    fseeko(file, 0, SEEK_END);
    file_size = ftello(file);
    fseeko(file, 0, SEEK_SET);

    int file_number = tox_file_send(tox, friend_number, TOX_FILE_KIND_DATA, file_size, filename, filename_length, &error);

    if(file_number != -1) {
        FILE_TRANSFER *file_handle = &active_transfer[friend_number][file_number];
        memset(file_handle, 0, sizeof(FILE_TRANSFER));


        file_handle->friend = friend_number;
        file_handle->file = file_number;
        file_handle->status = FILE_TRANSFER_STATUS_PAUSED_THEM;
        //TODO file_handle->sendsize = tox_file_data_size(tox, fid);

        file_handle->name = (uint8_t*)strdup((char*)filename);
        file_handle->path = (uint8_t*)strdup((char*)path);
        // name_lenght max size is FILE_T->name.
        filename_length = filename_length > sizeof(file_handle->name) ? sizeof(file_handle->name) : filename_length;
        file_handle->name_length = filename_length;

        file_handle->size = file_size;

        postmessage(FRIEND_FILE_OUT_NEW, friend_number, file_number, NULL);
        ++friend[friend_number].transfer_count;
        debug("Sending file %d of %d(max) to friend(%d).\n", friend[friend_number].transfer_count, MAX_FILE_TRANSFERS, friend_number);
    } else {
        debug("tox_file_send() failed\n");
    }
}

static void outgoing_file_callback_chunk(Tox *tox, uint32_t friend_number, uint32_t file_number, uint64_t position,
                                                                                        size_t length, void *user_data){

    debug("FileTransfer:\tChunk requested for friend_id (%u), and file_id (%u). Start (%u), End (%u).\r",
                                                                      friend_number, file_number, position, length);
    //send a chunk of data size of length with
    const uint8_t *data;
    uint8_t *buffer;
    buffer = malloc(length);

    FILE_TRANSFER *file_handle = &active_transfer[friend_number][file_number];
    FILE *file = file_handle->data;
    uint64_t last_bit = position + length;

    fseeko(file, 0, position);
    size_t read_size = fread(buffer, 1, length, file);
    data = buffer;

    if(read_size != length){
        debug("FileTransfer:\tERROR READING FILE! (%u & %u)\n", friend_number, file_number);
        tox_postmessage(TOX_FILE_OUTGOING_CANCEL, friend_number, file_number, NULL);
        return;
    }

    TOX_ERR_FILE_SEND_CHUNK error;
    tox_file_send_chunk(tox, friend_number, file_number, position, data, length, &error);
    if(last_bit == file_handle->size){
        debug("FileTransfer:\tOutgoing transfer is done (%u & %u)\n", friend_number, file_number);
    }
}

void utox_file_start_write(uint32_t friend_number, uint32_t file_number, void *filepath){
    FILE_TRANSFER *file_handle = &active_transfer[friend_number][file_number];
    file_handle->data = fopen(filepath, "wb");
    if(!file_handle->data) {
        free(filepath);
        file_handle->status = FILE_TRANSFER_STATUS_BROKEN;
        return;
    }
    file_handle->path = filepath;
    file_handle->path_length = strlen(filepath);
    file_handle->status = FILE_TRANSFER_STATUS_ACTIVE;
}

void utox_set_callbacks_for_transfer(Tox *tox){/*
    /* Incoming files */
        /* This is the callback for a new incoming file. */
        tox_callback_file_receive(tox, incoming_file_callback_request, NULL);
        /* This is the callback with friend's actions for a file */
        tox_callback_file_recv_control(tox, file_transfer_callback_control, NULL);
        /* This is the callback with a chunk data for a file. */
        tox_callback_file_receive_chunk(tox, incoming_file_callback_chunk, NULL);
    /* Outgoing files */
        /* This is the callback send to request a new file chunk */
        tox_callback_file_request_chunk(tox, outgoing_file_callback_chunk, NULL);
}
