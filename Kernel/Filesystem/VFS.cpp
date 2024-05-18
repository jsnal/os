#include <Kernel/Devices/PATADisk.h>
#include <Kernel/Devices/PartitionDevice.h>
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
    auto disk = PATADisk::create(PATADisk::Primary, PATADisk::Master);
    if (disk.ptr() == nullptr) {
        panic("Unable to find Disk!\n");
    }

    u8 mbr_header[512];
    disk->read_blocks(0, 1, mbr_header);
    u32 partition_offset = *((u32*)&mbr_header[0x1C6]);
    dbgprintf("VFS", "mbr_buf %x\n", partition_offset);

    auto partition_device = PartitionDevice(disk.leak_ptr(), partition_offset);

    auto root_filesystem = Ext2Filesystem(&partition_device);
    if (root_filesystem.init().is_error()) {
        panic("Unable to initialize root filesystem\n");
    }

    auto root_inode_id = root_filesystem.root_inode();
    auto root_inode = root_filesystem.inode(root_inode_id);

    if (!root_inode->is_directory()) {
        panic("Root filesystem is invalid\n");
    }

    dbgprintf("Kernel", "file contents: %s\n", buffer);
    m_root_inode = move(root_inode);

    dbgprintf("VFS", "VFS initialized\n");
}
