#include <bpf/libbpf.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static volatile int running = 1;

static void sig_handler(int sig) { running = 0; }

int main()
{
    struct bpf_object *obj;
    struct bpf_link *link;

    signal(SIGINT, sig_handler);

    obj = bpf_object__open_file("rofb-alpha-ebpf.o", NULL);
    if (libbpf_get_error(obj)) {
        fprintf(stderr, "Failed to open eBPF object\n");
        return 1;
    }

    bpf_object__load(obj);
    link = bpf_program__attach_kprobe(bpf_object__find_program_by_name(obj, "security_file_open"), false, NULL);

    printf("[ROFBSα] eBPF hook loaded on security_file_open. Press Ctrl+C to stop.\n");
    printf("[ROFBSα] Run ransomware in another terminal to test.\n\n");

    while (running) {
        sleep(1);
    }

    bpf_link__destroy(link);
    bpf_object__close(obj);
    printf("\n[ROFBSα] Hook unloaded.\n");
    return 0;
}
