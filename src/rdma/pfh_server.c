#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <infiniband/verbs.h>

int
main() {
    struct ibv_device **device_list; 
    struct ibv_context *ctx;
    struct ibv_pd *pd;
    struct ibv_mr *mr;
    int ndev, err;
    int i;
    char *pbuf;

    device_list = ibv_get_device_list(&ndev); 
    if (!device_list) {
        fprintf(stderr, "Error, ibv_get_device_list() failed\n");
        exit(1);
    } 
    for (i = 0; i < ndev; ++ i) {
        printf("RDMA device[%d]: name=%s\n", i, ibv_get_device_name(device_list[i]));
    } 
    printf("Open device %s\n", ibv_get_device_name(device_list[0]));

    ctx = ibv_open_device(device_list[0]);

    ibv_free_device_list(device_list);

    if (!ctx) {
        printf("Failed to open IB device.\n");
        exit(1);
    }
    printf("Create protected domain.\n");
    pd = ibv_alloc_pd(ctx);
    mr = ibv_reg_mr(pd, (void*)pbuf, 4096, IBV_ACCESS_REMOTE_WRITE);

    err = ibv_close_device(ctx);

    if (err != 0) {
        printf("Failed to close IB device. ERRNO = \n", errno);
        exit(1);
    }


    printf("IB device has been closed.\n");

    return 0;
    
}