#include "copytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#define BUFFER_SIZE 4096

void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    struct stat src_stat;
    int src_fd, dest_fd;
    ssize_t bytes_read, bytes_written;
    char buf[BUFFER_SIZE];

    // Get File Info using lstat (to see if it's a link)
    if (lstat(src, &src_stat) < 0) {
        perror("lstat failed");
        return;
    }



    // Handle Symbolic Links
    if (S_ISLNK(src_stat.st_mode)) {

        if (copy_symlinks) {
            char *link_target = malloc(src_stat.st_size + 1);
            if (!link_target) {
                perror("malloc failed");
                return;
            }

            ssize_t len = readlink(src, link_target, src_stat.st_size + 1);
            if (len < 0) {
                perror("readlink failed");
                free(link_target);
                return;
            }
            link_target[len] = '\0';

            if (symlink(link_target, dest) < 0) {
                perror("symlink failed");
            }

            free(link_target);
            return;   

        } else {
            // Follow the link.

            char target[1024];
            ssize_t len = readlink(src, target, sizeof(target)-1);
            if (len < 0) { 
                perror("readlink failed");
                return;
            }
            target[len] = '\0';

            // If target is relative, prepend the directory of src
            char full_target[4096];
            if (target[0] != '/') {
                char *last_slash = strrchr(src, '/');
                if (last_slash) {
                    size_t dir_len = last_slash - src;
                    snprintf(full_target, sizeof(full_target), "%.*s/%s", (int)dir_len, src, target);
                } else {
                    snprintf(full_target, sizeof(full_target), "%s", target);
                }
            } else {
                snprintf(full_target, sizeof(full_target), "%s", target);
            }

            copy_file(full_target, dest, copy_symlinks, copy_permissions);
            /* 
            if (stat(src, &src_stat) < 0) {
                perror("stat failed");
                return;
            }
            */
            return;
        }  
    }

    // Handle Regular Files
    if ((src_fd = open(src, O_RDONLY)) < 0) {
        perror("open src failed");
        return;
    }

    // Create file with 0644 (Read/Write) initially so we can write data.
    if ((dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
        perror("open dest failed");
        close(src_fd);
        return;
    }

    while ((bytes_read = read(src_fd, buf, sizeof(buf))) > 0) {
        bytes_written = write(dest_fd, buf, bytes_read);
        if (bytes_written != bytes_read) {
            perror("write failed");
            close(src_fd);
            close(dest_fd);
            return;
        }
    }

    if (bytes_read < 0) {
        perror("read failed");
    }

    close(src_fd);
    close(dest_fd);

    if (copy_permissions) {
        if (chmod(dest, src_stat.st_mode & 0777) < 0) {
            perror("chmod failed");
        }
    } 
}

void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {

    DIR *dir;
    struct dirent *entry;
    struct stat src_stat;
    char src_path[1024];
    char dest_path[1024];

    // Get info about the SOURCE directory
    if (lstat(src, &src_stat) < 0) {
        perror("lstat failed");
        return;
    }

    // Save mode for later
    mode_t source_mode = src_stat.st_mode;

    // Create Destination
    if (mkdir(dest, 0755) < 0) {
        if (errno != EEXIST) {
            perror("mkdir failed");
            return;
        }
    }

    // Open Source
    if ((dir = opendir(src)) == NULL) {
        perror("opendir failed");
        return;
    }

    // Recursion Loop
    while ((entry = readdir(dir)) != NULL) {


        // skip the previous directory and the current one.
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        if (lstat(src_path, &src_stat) < 0) {
            perror("lstat child failed");
            continue;
        }

        if (S_ISDIR(src_stat.st_mode)) {
            copy_directory(src_path, dest_path, copy_symlinks, copy_permissions);
        } else {
            copy_file(src_path, dest_path, copy_symlinks, copy_permissions);
        }
    }

    closedir(dir);

    // Apply Directory Permissions
    if (copy_permissions && !S_ISLNK(source_mode)) {
        if (chmod(dest, source_mode & 0777) < 0) {
            perror("chmod dir failed");
        }
    }
}
