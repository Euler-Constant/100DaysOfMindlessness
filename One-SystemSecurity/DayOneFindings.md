# Case Study 1: File-system detection technique against ransomware; ROFBS (System security)

Papers used: [arxiv:2603.16364v1](https://arxiv.org/pdf/2603.16364v1), [arxiv:2504.14162](https://arxiv.org/pdf/2504.14162)

Date: 2026-04-10
Status: Papers read, (will) recreate implementations in [eX-VFS](https://github.com/Euler-Constant/eX-VFS).

## First impressions:
While neck-deep in my personal (though incomplete) implementation of a Virtual File System (VFS), I ended up thinking a lot about threat vectors through the VFS itself. Surely enough, my suspicions ended up being correct; whatever protocol can encrypt files across board ends up being game over anyway if you're not faster than the attacker. So I scoured the internet, and found ROFBS and its asynchronous variant.

## Core Breakdown:

So, for a background (kind of), a filesystem is a data structure, visually representable as a tree. This models all data that can be accessed by a hard disk or a solid state drive. More complex definitions you can give to a filesystem is as a graph of interconnected objects, but for now we'd work with this. 
A virtual filesystem, well, you can call it another data structure, is a model that places a wide range of information about the hardware in the kernel, to represent different types of filesystems. I like to think they are hardware-contiguous, as well as filesystem-contiguous, as long as it is supported by the VFS in question. 
On the Linux kernel there are a lot of filesystems that are supported by the VFS, but how exactly? The Common File Model is one such abstraction layer of sorts, as it is meant to translate the physical organisation of the sectors into the VFS. Much more precisely though, the kernel can't particularly hard-code a particular function to handle operations (such as the 'read()' operation), but instead uses a pointer for each operation, which, if you haven't caught up on the flow of things by now, is a syscall. This would be important for the rest of this case study. The filesystem is also object-oriented, and not in the paradigm sense, but in the way that it is a construct that defines both a data structure and its methods.

With the introduction of all of that, and its necessity, threat actors over the timeline of the internet-of-things have found rather, creative ways, to say the least, to manipulating these things, and one of them is the advent of the financially motivated ransomware. What ROFBS and ROFBS(alpha) does in response to that regard, is that it backs up pre-modification data in real-time upon 'file-open events', which, if you can piece together, is a syscall, and 'aims for high recoverability against ransomware-induced encryption by obtaining backups before files are modified'. You might be wondering, as much as I did, where the 'control unit' is. The control unit is an LSM, a Mandatory Access Control (MAC) framework that allows for various security modules to be 'enforced'. It is primarily the question of "May a subject S (or function F) perform a kernel operation OP on an internal kernel object OBJ"

![Algorithm implementation](https://github.com/user-attachments/assets/1565cc79-c681-445b-b751-246b7cc91d01)

More underlying principles do exist, like eBPF, that extends kernel capabilities without modifying source code through eBPF hooks, and more specific techniques like modifying the file-open path, though that would be better off discussed with an implementation.

With this backdrop, we could then study ROFBS and ROFBS(alpha) (its async variant), in further detail.

![ROFBS(alpha) model](https://github.com/user-attachments/assets/78039b26-d357-4f60-9cf9-986eef5fed4a)

