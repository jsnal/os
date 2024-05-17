#include <Kernel/Devices/PATADisk.h>
#include <Kernel/Filesystem/Ext2Filesystem.h>
#include <Kernel/Filesystem/VFS.h>
#include <Kernel/Logger.h>

VFS& VFS::the()
{
    static VFS m_the;
    return m_the;
}

VFS::VFS()
{
}

void VFS::init()
{
    auto disk = PATADisk::create(PATADisk::Secondary, PATADisk::Master);
    if (disk.ptr() == nullptr) {
        panic("Unable to find Disk!\n");
    }

    auto root_filesystem = Ext2Filesystem(disk.leak_ptr());
    if (root_filesystem.init().is_error()) {
        panic("Unable to initialize root filesystem\n");
    }

    auto root_inode_id = root_filesystem.root_inode();
    auto root_inode = root_filesystem.inode(root_inode_id);

    if (!root_inode->is_directory()) {
        panic("Root filesystem is invalid\n");
    }

    m_root_inode = move(root_inode);

    dbgprintf("VFS", "VFS initialized\n");
}
