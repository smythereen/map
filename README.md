# map — cached memory-mapped files

`map` lets a C program treat a file as memory. You give it a path, an
offset and a length, and it hands back a pointer to a `MAP_SHARED`
`mmap(2)` region. The file is created or grown if it needs to be. Every
process that maps the same file sees the same bytes, so the system uses
`map` for shared-memory segments and persistent tables. It saves you
from writing open/stat/ftruncate/mmap/munmap code in each program.

## What it does

- **Path resolution.** Paths that start with `/` or `.` are used as
  given. Bare names go to `File_Path(..., "MEMORY_PATH", name)` from
  `sys`, so they resolve under the project's memory directory.
- **Create and resize.** When `PROT_WRITE` is set and the file is
  missing, `map` creates the parent directories (`MkDir`) and the
  file, then sizes it to `offset + length`. A file that is too short
  always grows. It shrinks only when `MAP_SHRINK` is set.
- **Mapping cache.** An internal table (`MapTbl`, 200 slots) tracks each
  mapping by device, inode, size, offset, length and protection. If you
  map the same region again, you get the cached address back without a
  new `mmap`. If the file's size or the request has changed, the old
  slot is unmapped and replaced. When every slot is used, the oldest
  logically released slot is reused.
- **Two kinds of unmap.** With `force_flag == 0`, the slot is only
  marked unused (logical unmap), so a later `Map` can reuse the
  address cheaply. With `force_flag != 0`, the region is removed with
  `munmap` (physical unmap). `UnmapAll()` physically removes every
  slot that is logically released.
- **Optional per-file lock.** `MapFileEx()` takes a `sem` semaphore
  keyed on the file's inode (`SemAllo`) before mapping. Every unmap
  path calls `SemFree(inode)`.
- **Sync and timestamps.** `UpdateMap(adr,len)` runs
  `msync(MS_ASYNC|MS_INVALIDATE)` on a mapped range.
  `MapFileSetTimes()` sets atime and mtime. After each `Map`, the
  globals `MapFileSize`, `MapFileAccessTime` and `MapFileModifiedTime`
  describe the mapping.

## API (`map.h`)

| Function | Purpose |
|---|---|
| `void *Map(char *file, s64 offset, s32 length, u32 prot)` | Core call. `prot` is `PROT_WRITE` or 0, optionally ORed with `MAP_SHRINK`. `offset=0,length=0` maps the whole existing file. A negative `offset` treats `length` as a change to the file size. Returns `NULL` on error, which is logged with `ErrorLog`. |
| `void *MapFile(char *file)` | Maps an existing file read/write in full: `Map(file,0,0,PROT_WRITE)`. |
| `void *MapFileEx(char *file)` | Same as `MapFile`, but first locks the inode semaphore through `sem`. |
| `void *MapFile2Mem(char *file, off_t len, u32 prot)` | Maps exactly `len` bytes and sets the file to that size (`MAP_SHRINK`). |
| `void UpdateMap(void *adr, off_t len)` | Flushes a mapped range with `msync`. |
| `int MapFileSetTimes(char *file, time_t *atime, time_t *mtime)` | Sets file times. Pass `NULL` to keep a time unchanged. |
| `void UnmapAddress(void *adr, int force)` / `UnmapFile(char *file, int force)` / `UnmapInode(ino_t, int force)` | Release one mapping, found by address, path or inode. |
| `void UnmapAll(void)` | Physically unmap every logically released slot. |

`mapgrow.c` also defines `MapAndGrowFile(file, delta)`, which is
`Map(file,-1,delta,PROT_WRITE[|MAP_SHRINK])`. It is **not declared** in
`map.h`. The trace bit is `MAP_TRC_LVL` (0x08000000).

## Directory and build layout

```
map/
  map.h          public header, installed to ~/cmn by the unit rules
  maplocal.h     private: map_t, MapTbl[200], Unmap()
  map.c          Map() and the cache (MapTbl and global variables)
  mapfile.c mapfileex.c mapmem.c mapgrow.c mapupd.c mapfiletime.c
  unmap.c unmapaddr.c unmapall.c unmapfile.c unmapino.c
  testmap.c      test program (has main)
  makefile       includes ../rules.mak and ../targets.mak
  UNIT.deps      base 1, require sem, require sys
```

Build it inside a `base` project tree, after `sys` and `sem`. Fetch
it with `repo add map`, then run `make` in `map/`. The makefile sets
`CHDRS = $(HDR)/map.h` and `LIBRARIES = -lsem -lsys`. `libmap` itself
is added by the shared rules.

## Example (from `testmap.c`)

`testmap file [offset [size]]` maps the file with
`PROT_WRITE|MAP_SHRINK` and fills it with `A%06u\n` records. It then
sleeps 30 s, remaps using a negative offset to grow the file by its own
size, writes `B%06u` records, and sleeps 60 s. While it sleeps, you can
inspect the file from another process. Validated run (in a temporary
copy, with tracing on):

```
$ testmap ./demo.dat 0 64
26-11-05 22:01:01.188424> Map(./demo.dat,offset=0,length=64,prot=0x10002)
26-11-05 22:01:01.188480> Slot   0, Inode      0, Address 0x3BAA5000, Size 64 - Mapped File "./demo.dat"
$ head -c 64 demo.dat        # seen from a second process
A000000
A000008
...
A000056
```

Minimal use:

```c
#include <map.h>
char *p = Map("counters.dat", 0, 4096, PROT_WRITE); /* created if missing */
p[0]++;                                             /* shared with other processes */
UpdateMap(p, 4096);
UnmapAddress(p, 0);                                 /* logical release, cached */
```

## Dependencies

- **Direct:** `sem` (`SemAllo`, `SemFree`) and `sys` (`ErrorLog`,
  `Trace`/`SysPrt`, `File_Path`, `MkDir`, `LocalName`,
  `TraceLevel`, `TracePointer`, plus the `common.h` types). Checked by
  relinking: without `-lsem` the link fails with an undefined
  `SemFree`. Without `-lsys` it fails on the `sys` symbols.
- **Indirect (transitive):** map → sem → sys. sys has no public-unit
  dependencies, so the full set is {sem, sys}.
- **Makefile `-l` list vs. real use:** `-lsem -lsys` matches what the
  code actually uses, with no unused entries. The shared `rules.mak`
  default `LIBRARIES` also lists `libmsg` and `libfar`, but this
  makefile overrides that list, and `map` uses neither library.
  `testprot.c` has been removed, so nothing in this unit refers to any
  other unit.

## License

The original files are offered under `Apache-2.0 OR GPL-2.0-or-later`;
you choose either license. See `LICENSE` and `LICENSES/`. Copyright
notices are retained; see the SPDX identifier in each source file.

## Version

Version tag: `v0.2.0`.
