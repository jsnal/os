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

    u8 master_boot_record[512];
    disk->read_blocks(0, 1, master_boot_record);
    u32 partition_offset = *((u32*)&master_boot_record[0x1C6]);

    auto partition_device = PartitionDevice::create(disk.leak_ptr(), partition_offset);

    m_root_filesystem = make_unique_ptr<Ext2Filesystem>(partition_device.leak_ptr());
    if (m_root_filesystem.ptr() == nullptr) {
        panic("Unable to create root filesystem\n");
    }

    if (root_filesystem().init().is_error()) {
        panic("Unable to initialize root filesystem\n");
    }

    auto root_inode_id = root_filesystem().root_inode();
    auto root_inode = root_filesystem().inode(root_inode_id);

    if (!root_inode->is_directory()) {
        panic("Root filesystem is invalid\n");
    }

    m_root_inode = move(root_inode);

    open("/home/user/file.txt", 0);

    dbgprintf("VFS", "VFS initialized\n");
}

ResultOr<u32> VFS::open(const String& path, mode_t mode)
{
    auto path_traversal_result = traverse_path(path, m_root_inode);
    return 0;
}

ResultOr<SharedPtr<Inode>> VFS::traverse_path(const String& path, SharedPtr<Inode>& base)
{
    dbgprintf("VFS", "Starting to traverse the path for '%s'\n", path.str());

    auto split_path = path.split('/');
    InodeId current_inode_id;

    for (size_t i = 0; i < split_path.size(); i++) {
        dbgprintf("VFS", "path[%u]='%s'\n", i, split_path[i].str());

        dbgprintf("VFS", "base->id=%u\n", base->id());
        auto find_result = base->find(split_path[i]);
        if (find_result.is_error()) {
            return find_result.error();
        }

        dbgprintf("VFS", "Found entry @ inode %u\n", current_inode_id.id());

        current_inode_id = find_result.value();
        base = root_filesystem().inode(current_inode_id);
    }

    return m_root_inode;
}
