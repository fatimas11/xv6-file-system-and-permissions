# 📁 File System Enhancements: Recursive Tree Copying & xv6 File Permissions

## 📌 Overview
This repository contains solutions for the final coursework in **Operating Systems** at Bar-Ilan University. The project focuses on file system architecture across two levels: implementing a POSIX-compliant **Recursive Directory Tree Copying Utility** in C, and extending the **xv6-riscv** OS kernel with a complete **File Permissions System**.

---

## 🧩 Project Modules

### 1. User-Space Recursive Tree Copying (`copytree/`)
Implements a user-level directory traversal and duplication library (`copytree.c`, `copytree.h`).
* Recurses through nested directories, copying subdirectories, files, and symlinks.
* Preserves original file metadata, access modes, and file permission bits during duplication.
* Implements robust error handling for broken links and circular references.

---

### 2. xv6 Kernel File Permissions & Bonus (`xv6-permissions-bonus/`)
Extends the **xv6-riscv** kernel to enforce file access controls and execution permissions (`rwx` flags).
* **Inode & File System Extensions (`fs.c`, `fs.h`, `file.h`):** Modified the on-disk and in-memory Inode structures to store and persist file permission mode bits.
* **Access Control Enforcement (`sysfile.c`, `exec.c`):** Integrated permission checks on file system calls (`open`, `read`, `write`) and binary execution (`exec`).
* **Comprehensive Test Suite (`user/`):** Includes an extensive set of test scripts verifying permission persistence, execution checks, edge cases, and security boundary enforcement:
  * `testperm.c`
  * `testexec.c`
  * `testpermscenarios.c`
  * `testpermedge.c`
  * `testpermpersist.c`
  * `testpermcomprehensive.c`

---

## 🛠️ Tech Stack & Concepts
* **Languages:** C (POSIX Standard), Assembly
* **Environment:** xv6-riscv Kernel, Linux Terminal, QEMU RISC-V Emulator
* **Concepts:** File Systems, Inodes, Access Control Lists (ACLs), POSIX Permissions, Recursive Directory Traversal, System Call Integration

---

## 🚀 How to Run

### Building Copytree Utility:
```bash
gcc -Wall copytree.c part3.c -o copytree
./copytree [source_dir] [target_dir]
