# Day Five: Now, this is all just speculation, but-

- Documentation: [Encryption techniques](https://www.morphisec.com/blog/breaking-down-ransomware-encryption-key-strategies-algorithms-and-implementation-trends/), [Targeting backups before encryption](https://www.baculasystems.com/blog/why-ransomware-target-backups-before-production/), [Workarounds against encrypted backups](https://www.findarticles.com/ai-ransomware-undermines-encrypted-backups/)
- Date: 2026-04-15
- Status: Actually getting to understand it now (Articles read)

## Core Breakdown:

This might as well be speculation (Probably all just speculation), but I thought of increasing the attack surface for the ransomware while working on the model today and I got to get an intereesting perspective on the attack range:

Round 1
Workaround: Ransomware is updated to also encrypt .tmp files. Simply, an attacker reads about this defense and adds .tmp to its target extensions list.
Patch: Instead of a fixed .tmp extension, the backup system uses a randomized, unpredictable extension generated at runtime (e.g., .x7q2m). Ransomware can't hardcode an avoidance list against something it doesn't know.

Round 2
Workaround: Ransomware scans the directory first, fingerprints the backup files by their file structure or metadata (creation timestamp clustering, identical size to the original, etc.) rather than extension, and targets those.
Patch: The backup file is padded to a different size and its timestamps are randomized to not correlate with the original. Metadata fingerprinting becomes unreliable.

Round 3
Workaround: Ransomware doesn't look at individual files at all. It just encrypts everything in the directory indiscriminately, extensions and metadata be damned.
Patch: Backups are moved out of the victim directory entirely — into a hidden system-level directory with strict filesystem permissions that the ransomware process cannot access under its privilege level.

Round 4
Workaround: Ransomware runs with elevated privileges (which many real-world ransomware strains already do via privilege escalation exploits), bypassing filesystem permission restrictions.
Patch: The backup directory is protected not just by filesystem permissions but by a separate eBPF hook that blocks any write or delete syscall to that directory from any process other than the backup daemon itself — essentially a kernel-level access control wall.

Round 5
Workaround: Rather than touching the backup files directly, ransomware targets the backup daemon process itself — kills it before it can back up anything, or corrupts its memory.
Patch: The backup daemon is given a watchdog process that relaunches it if it dies, and the daemon's memory is isolated. Better yet, the backup logic is pushed further down into a kernel module or eBPF program itself, making it harder to kill from userspace.

Round 6
Workaround: Ransomware exploits the suspend/resume window in the ROFBSα model. Recall that the process is briefly suspended while the backup is created. A sophisticated ransomware could spawn multiple parallel threads that flood the system with file open events simultaneously, overwhelming the backup daemon so that many files get suspended but backups can't be created fast enough before the queue overflows.
Patch: The backup system implements a priority queue with rate limiting — capping how many backup operations can be pending at once, and prioritizing files by type (documents over executables, for example). Combined with multi-threaded backup workers, throughput scales with the attack volume.

Round 7
Workaround: Ransomware avoids opening files altogether. Instead of a traditional read-encrypt-write cycle, it uses memory-mapped I/O (mmap) to manipulate file contents directly in memory, potentially bypassing the xfs_file_open hook that ROFBSα relies on.
Patch: Extend the eBPF hooks to also cover mmap and mprotect syscalls, not just file open events. Any memory mapping of a protected file triggers the same backup procedure.

Round 8
Workaround: Ransomware operates at the block device level — writing encrypted data directly to disk sectors, bypassing the filesystem and all its syscalls entirely. No open, no mmap, nothing the eBPF hooks are watching.
Patch: This is where the software model starts hitting a hard ceiling. The patch here would require moving protection to the block device driver layer itself, or using a hypervisor-level monitor (if the system runs in a VM) to intercept raw disk writes. This is a fundamentally different and more complex system.

It may be worth knowing that attack surfaces above round 3 go into 'nukeware' territory, as I have conveniently termed, and examples have actually been sighted amongst enterprise grade ransomware.
