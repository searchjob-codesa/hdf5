/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the LICENSE file, which can be found at the root of the source code       *
 * distribution tree, or in https://www.hdfgroup.org/licenses.               *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "hdf5.h"
#include "H5VLpublic.h"
#include "H5VLconnector.h"
#include "H5Tpublic.h"
#include "H5Ppublic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define H5_FAILED() {printf("*FAILED*\n"); fflush(stdout);}
#define H5_PASSED() {printf(" PASSED\n"); fflush(stdout);}
#define TESTING(WHAT) {printf("Testing %-62s", WHAT); fflush(stdout);}
#define TEST_ERROR {goto error;}

#define H5VL_DATATYPE_GET_TEST_FILE_NAME "test_H5VLdatatype_get.h5"
#define H5VL_DATATYPE_GET_TEST_DTYPE_NAME "committed_dtype"

/* Sequence: H5VLdatatype_get */

static int
test_H5VLdatatype_get(void)
{
    hid_t file_id = H5I_INVALID_HID;
    hid_t dtype_id = H5I_INVALID_HID;
    hid_t committed_dtype_id = H5I_INVALID_HID;
    hid_t native_vol_id = H5I_INVALID_HID;
    hid_t dxpl_id = H5I_INVALID_HID;
    hid_t lcpl_id = H5I_INVALID_HID;
    hid_t tcpl_id = H5I_INVALID_HID;
    hid_t tapl_id = H5I_INVALID_HID;
    H5VL_datatype_get_args_t args;
    herr_t ret_value = -1;
    void *obj = NULL;
    void **req = NULL;
    size_t binary_size = 0;

    printf("[DEBUG] Entering test_H5VLdatatype_get function\n");
    fflush(stdout);

    TESTING("H5VLdatatype_get");

    /* Create a simple file to get a valid datatype */
    printf("[DEBUG] About to create file: %s\n", H5VL_DATATYPE_GET_TEST_FILE_NAME);
    fflush(stdout);
    if ((file_id = H5Fcreate(H5VL_DATATYPE_GET_TEST_FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0) {
        printf("[DEBUG] H5Fcreate failed, file_id = %ld\n", (long)file_id);
        fflush(stdout);
        H5_FAILED();
        printf("    couldn't create file\n");
        goto error;
    }
    printf("[DEBUG] H5Fcreate succeeded, file_id = %ld\n", (long)file_id);
    fflush(stdout);

    /* Create a datatype */
    printf("[DEBUG] About to copy H5T_NATIVE_INT datatype\n");
    fflush(stdout);
    if ((dtype_id = H5Tcopy(H5T_NATIVE_INT)) < 0) {
        printf("[DEBUG] H5Tcopy failed, dtype_id = %ld\n", (long)dtype_id);
        fflush(stdout);
        H5_FAILED();
        printf("    couldn't copy datatype\n");
        goto error;
    }
    printf("[DEBUG] H5Tcopy succeeded, dtype_id = %ld\n", (long)dtype_id);
    fflush(stdout);

    /* Create property lists for committing the datatype */
    printf("[DEBUG] About to create link creation property list\n");
    fflush(stdout);
    if ((lcpl_id = H5Pcreate(H5P_LINK_CREATE)) < 0) {
        printf("[DEBUG] H5Pcreate(H5P_LINK_CREATE) failed, lcpl_id = %ld\n", (long)lcpl_id);
        fflush(stdout);
        H5_FAILED();
        printf("    couldn't create link creation property list\n");
        goto error;
    }
    printf("[DEBUG] H5Pcreate(H5P_LINK_CREATE) succeeded, lcpl_id = %ld\n", (long)lcpl_id);
    fflush(stdout);

    printf("[DEBUG] About to create datatype creation property list\n");
    fflush(stdout);
    if ((tcpl_id = H5Pcreate(H5P_DATATYPE_CREATE)) < 0) {
        printf("[DEBUG] H5Pcreate(H5P_DATATYPE_CREATE) failed, tcpl_id = %ld\n", (long)tcpl_id);
        fflush(stdout);
        H5_FAILED();
        printf("    couldn't create datatype creation property list\n");
        goto error;
    }
    printf("[DEBUG] H5Pcreate(H5P_DATATYPE_CREATE) succeeded, tcpl_id = %ld\n", (long)tcpl_id);
    fflush(stdout);

    printf("[DEBUG] About to create datatype access property list\n");
    fflush(stdout);
    if ((tapl_id = H5Pcreate(H5P_DATATYPE_ACCESS)) < 0) {
        printf("[DEBUG] H5Pcreate(H5P_DATATYPE_ACCESS) failed, tapl_id = %ld\n", (long)tapl_id);
        fflush(stdout);
        H5_FAILED();
        printf("    couldn't create datatype access property list\n");
        goto error;
    }
    printf("[DEBUG] H5Pcreate(H5P_DATATYPE_ACCESS) succeeded, tapl_id = %ld\n", (long)tapl_id);
    fflush(stdout);

    /* Commit the datatype to the file */
    printf("[DEBUG] About to commit datatype '%s' to file\n", H5VL_DATATYPE_GET_TEST_DTYPE_NAME);
    fflush(stdout);
    if ((ret_value = H5Tcommit2(file_id, H5VL_DATATYPE_GET_TEST_DTYPE_NAME, dtype_id, lcpl_id, tcpl_id, tapl_id)) < 0) {
        printf("[DEBUG] H5Tcommit2 failed, ret_value = %d\n", ret_value);
        fflush(stdout);
        H5_FAILED();
        printf("    couldn't commit datatype\n");
        goto error;
    }
    printf("[DEBUG] H5Tcommit2 succeeded, ret_value = %d\n", ret_value);
    fflush(stdout);

    /* Open the committed datatype */
    printf("[DEBUG] About to open committed datatype '%s'\n", H5VL_DATATYPE_GET_TEST_DTYPE_NAME);
    fflush(stdout);
    if ((committed_dtype_id = H5Topen2(file_id, H5VL_DATATYPE_GET_TEST_DTYPE_NAME, tapl_id)) < 0) {
        printf("[DEBUG] H5Topen2 failed, committed_dtype_id = %ld\n", (long)committed_dtype_id);
        fflush(stdout);
        H5_FAILED();
        printf("    couldn't open committed datatype\n");
        goto error;
    }
    printf("[DEBUG] H5Topen2 succeeded, committed_dtype_id = %ld\n", (long)committed_dtype_id);
    fflush(stdout);

    /* Get the native VOL connector ID */
    printf("[DEBUG] About to get native VOL connector ID\n");
    fflush(stdout);
    if ((native_vol_id = H5VLget_connector_id_by_name("native")) < 0) {
        printf("[DEBUG] H5VLget_connector_id_by_name failed, native_vol_id = %ld\n", (long)native_vol_id);
        fflush(stdout);
        H5_FAILED();
        printf("    couldn't get native VOL connector ID\n");
        goto error;
    }
    printf("[DEBUG] H5VLget_connector_id_by_name succeeded, native_vol_id = %ld\n", (long)native_vol_id);
    fflush(stdout);

    /* Get the object from the committed datatype ID */
    printf("[DEBUG] About to get VOL object from committed datatype\n");
    fflush(stdout);
    if ((obj = H5VLobject(committed_dtype_id)) == NULL) {
        printf("[DEBUG] H5VLobject failed, obj = %p\n", obj);
        fflush(stdout);
        H5_FAILED();
        printf("    couldn't get VOL object from committed datatype\n");
        goto error;
    }
    printf("[DEBUG] H5VLobject succeeded, obj = %p\n", obj);
    fflush(stdout);

    /* Create a data transfer property list */
    printf("[DEBUG] About to create data transfer property list\n");
    fflush(stdout);
    if ((dxpl_id = H5Pcreate(H5P_DATASET_XFER)) < 0) {
        printf("[DEBUG] H5Pcreate(H5P_DATASET_XFER) failed, dxpl_id = %ld\n", (long)dxpl_id);
        fflush(stdout);
        H5_FAILED();
        printf("    couldn't create data transfer property list\n");
        goto error;
    }
    printf("[DEBUG] H5Pcreate(H5P_DATASET_XFER) succeeded, dxpl_id = %ld\n", (long)dxpl_id);
    fflush(stdout);

    /* Set up the get args for binary size query */
    printf("[DEBUG] Setting up H5VL_datatype_get_args_t structure\n");
    fflush(stdout);
    memset(&args, 0, sizeof(H5VL_datatype_get_args_t));
    args.op_type = H5VL_DATATYPE_GET_BINARY_SIZE;
    args.args.get_binary_size.size = &binary_size;
    printf("[DEBUG] args.op_type = %d, binary_size pointer = %p\n", args.op_type, (void*)&binary_size);
    fflush(stdout);

    /* Call H5VLdatatype_get */
    printf("[DEBUG] About to call H5VLdatatype_get\n");
    fflush(stdout);
    if ((ret_value = H5VLdatatype_get(obj, native_vol_id, &args, dxpl_id, req)) < 0) {
        printf("[DEBUG] H5VLdatatype_get failed, ret_value = %d\n", ret_value);
        fflush(stdout);
        H5_FAILED();
        printf("    H5VLdatatype_get returned error\n");
        goto error;
    }
    printf("[DEBUG] H5VLdatatype_get succeeded, ret_value = %d\n", ret_value);
    fflush(stdout);

    /* Validate that binary_size was populated with a positive value */
    printf("[DEBUG] Checking binary_size value: binary_size = %zu\n", binary_size);
    fflush(stdout);
    if (binary_size == 0) {
        printf("[DEBUG] binary_size is 0, expected positive value\n");
        fflush(stdout);
        H5_FAILED();
        printf("    binary size is 0, expected positive value\n");
        goto error;
    }
    printf("[DEBUG] binary_size validation passed: binary_size = %zu\n", binary_size);
    fflush(stdout);

    printf("[DEBUG] About to close dxpl_id\n");
    fflush(stdout);
    if (H5Pclose(dxpl_id) < 0) {
        printf("[DEBUG] H5Pclose(dxpl_id) failed\n");
        fflush(stdout);
        TEST_ERROR;
    }
    printf("[DEBUG] H5Pclose(dxpl_id) succeeded\n");
    fflush(stdout);

    printf("[DEBUG] About to close native_vol_id\n");
    fflush(stdout);
    if (H5VLclose(native_vol_id) < 0) {
        printf("[DEBUG] H5VLclose(native_vol_id) failed\n");
        fflush(stdout);
        TEST_ERROR;
    }
    printf("[DEBUG] H5VLclose(native_vol_id) succeeded\n");
    fflush(stdout);

    printf("[DEBUG] About to close committed_dtype_id\n");
    fflush(stdout);
    if (H5Tclose(committed_dtype_id) < 0) {
        printf("[DEBUG] H5Tclose(committed_dtype_id) failed\n");
        fflush(stdout);
        TEST_ERROR;
    }
    printf("[DEBUG] H5Tclose(committed_dtype_id) succeeded\n");
    fflush(stdout);

    printf("[DEBUG] About to close tapl_id\n");
    fflush(stdout);
    if (H5Pclose(tapl_id) < 0) {
        printf("[DEBUG] H5Pclose(tapl_id) failed\n");
        fflush(stdout);
        TEST_ERROR;
    }
    printf("[DEBUG] H5Pclose(tapl_id) succeeded\n");
    fflush(stdout);

    printf("[DEBUG] About to close tcpl_id\n");
    fflush(stdout);
    if (H5Pclose(tcpl_id) < 0) {
        printf("[DEBUG] H5Pclose(tcpl_id) failed\n");
        fflush(stdout);
        TEST_ERROR;
    }
    printf("[DEBUG] H5Pclose(tcpl_id) succeeded\n");
    fflush(stdout);

    printf("[DEBUG] About to close lcpl_id\n");
    fflush(stdout);
    if (H5Pclose(lcpl_id) < 0) {
        printf("[DEBUG] H5Pclose(lcpl_id) failed\n");
        fflush(stdout);
        TEST_ERROR;
    }
    printf("[DEBUG] H5Pclose(lcpl_id) succeeded\n");
    fflush(stdout);

    printf("[DEBUG] About to close dtype_id\n");
    fflush(stdout);
    if (H5Tclose(dtype_id) < 0) {
        printf("[DEBUG] H5Tclose(dtype_id) failed\n");
        fflush(stdout);
        TEST_ERROR;
    }
    printf("[DEBUG] H5Tclose(dtype_id) succeeded\n");
    fflush(stdout);

    printf("[DEBUG] About to close file_id\n");
    fflush(stdout);
    if (H5Fclose(file_id) < 0) {
        printf("[DEBUG] H5Fclose(file_id) failed\n");
        fflush(stdout);
        TEST_ERROR;
    }
    printf("[DEBUG] H5Fclose(file_id) succeeded\n");
    fflush(stdout);

    H5_PASSED();

    printf("[DEBUG] Exiting test_H5VLdatatype_get function successfully (returning 0)\n");
    fflush(stdout);
    return 0;

error:
    printf("[DEBUG] Entered error cleanup section\n");
    fflush(stdout);
    H5E_BEGIN_TRY
    {
        H5Pclose(dxpl_id);
        H5VLclose(native_vol_id);
        H5Tclose(committed_dtype_id);
        H5Pclose(tapl_id);
        H5Pclose(tcpl_id);
        H5Pclose(lcpl_id);
        H5Tclose(dtype_id);
        H5Fclose(file_id);
    }
    H5E_END_TRY
    printf("[DEBUG] Completed error cleanup\n");
    fflush(stdout);

    printf("[DEBUG] Exiting test_H5VLdatatype_get function with error (returning -1)\n");
    fflush(stdout);
    return -1;
}

int
main(void)
{
    int nerrors = 0;

    printf("[DEBUG] Starting main function\n");
    fflush(stdout);

    printf("**********************************************\n");
    printf("*                                            *\n");
    printf("*        API H5VLdatatype_get Tests          *\n");
    printf("*                                            *\n");
    printf("**********************************************\n\n");

    printf("[DEBUG] About to call test_H5VLdatatype_get\n");
    fflush(stdout);
    nerrors += test_H5VLdatatype_get() < 0 ? 1 : 0;
    printf("[DEBUG] test_H5VLdatatype_get returned, nerrors = %d\n", nerrors);
    fflush(stdout);

    printf("\n");

    if (nerrors > 0) {
        printf("[DEBUG] Exiting main with EXIT_FAILURE (nerrors = %d)\n", nerrors);
        fflush(stdout);
        return EXIT_FAILURE;
    } else {
        printf("[DEBUG] Exiting main with EXIT_SUCCESS (nerrors = %d)\n", nerrors);
        fflush(stdout);
        return EXIT_SUCCESS;
    }
}