// ROFBSα-style decoupled backup hook

#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <linux/fs.h>

struct backup_event {
    __u64 timestamp_ns;
    __u32 pid;
    char filename[256];
    __u8 success;        // 1 = backup successfully created, 0 = backup failed
};

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 1 << 20);  // 1MB ring buffer (still do not get this)
} events SEC(".maps"); 

SEC("kprobe/security_file_open")
int BPF_KPROBE(security_file_open, struct file *file)
{
    struct backup_event *e;
    struct path *path = &file->f_path;
    char *filename;

    e = bpf_ringbuf_reserve(&events, sizeof(*e), 0);
    if (!e)
        return 0;

    e->timestamp_ns = bpf_ktime_get_ns();
    e->pid = bpf_get_current_pid_tgid() >> 32;

    // Get filename
    filename = (char *)bpf_get_current_comm();  // fallback
    bpf_probe_read_kernel_str(e->filename, sizeof(e->filename), path->dentry->d_name.name);

    e->success = 1;   // TODO: replace with actual copy_to_tmp()

    bpf_ringbuf_submit(e, 0);
    return 0;
}

char _license[] SEC("license") = "GPL";
