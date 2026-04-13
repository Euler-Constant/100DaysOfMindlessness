# Day Three: The File-open path, and implementation goals

- Paper: (Papers in repo)
- Date: 2026-04-13
- Status: Confuzzled (Paper/documentation read)

## Core Breakdown:
(-What other file-open paths could there be to exploit?)
Coming back to the issue of the request/function/object needing to 'parse' as a syscall first, the ROFBS paper summarizes a 'model path' for kernel hook points from the initial call to the response signal given by the file-system:

### may_open: 
This is considered a "decision function", and for all intents and purposes, it's hooked to 'observe events before the actual open execution and before the file-system specific callback is reached', which I imagine to be the where the model kicks in its 'detection phase'. It includes events that do not proceed to later stages, which makes sense due to the varying backup ratios while using different ransomware families.

### inode_permission: 
This performs permission checks, and I was correct with my initial analysis that it indeed is in sync with LSM hook initialisation. It's invoked from *may_open*, but it is not specific to open operations. 
- Implementation issues: invocations caused specifically by the open path
- Because this function primarily handles an inode and an access mask, and does not provide path-based or file-based context, the information provided at this point is limited.)

### do_dentry_open: 
Located at the common VFS open-execution stage. This point marks major fields in the file structure already being initialised, which makes it less of an operation point and more of a identification scan point.

### security_file_open:
I somehow get this and somehow do not, grand scheme of things. It is executed in *do_dentry_open*, before the file-system specific node is invoked, in this study's case, xfs_file-open, which is the callback for regular files. It is the entry point for LSM-based state handling (which I imagine to be synchronous?). Invoking this hook allows events to be captured at a point close to the actual operation while remaining on the file-system independent common path.

### xfs_file-open:
The file-system specific open callback, and the callback used in this case study.

The main ideas from this is during this process, the model takes a simplified path through path resolution, acquisition of dentries and inodes, access checks and confirmations, and initialisation of the file structure. 

![Screenshot_2026-04-13_21-52-39](https://github.com/user-attachments/assets/8b62809b-3ad6-4cf6-92a3-42c18e815975)

