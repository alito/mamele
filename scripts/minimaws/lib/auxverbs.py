#!/usr/bin/python
##
## license:BSD-3-Clause
## copyright-holders:Vas Crabb

from . import dbaccess

import codecs
import hashlib
import os
import os.path
import struct
import sys
import zlib


class _Identifier(object):
    def __init__(self, dbcurs, **kwargs):
        super(_Identifier, self).__init__(**kwargs)
        self.dbcurs = dbcurs
        self.pathwidth = 0
        self.labelwidth = 0
        self.matches = { }
        self.unmatched = [ ]

    def processRomFile(self, path, f):
        crc, sha1 = self.digestRom(f)
        matched = False
        for shortname, description, label, bad in self.dbcurs.get_rom_dumps(crc, sha1):
            matched = True
            self.labelwidth = max(len(label), self.labelwidth)
            romset = self.matches.get(shortname)
            if romset is None:
                romset = (description, [])
                self.matches[shortname] = romset
            romset[1].append((path, label, bad))
        if not matched:
            self.unmatched.append((path, crc, sha1))
        self.pathwidth = max(len(path), self.pathwidth)

    def processChd(self, path, sha1):
        matched = False
        for shortname, description, label, bad in self.dbcurs.get_disk_dumps(sha1):
            matched = True
            self.labelwidth = max(len(label), self.labelwidth)
            romset = self.matches.get(shortname)
            if romset is None:
                romset = (description, [])
                self.matches[shortname] = romset
            romset[1].append((path, label, bad))
        if not matched:
            self.unmatched.append((path, None, sha1))
        self.pathwidth = max(len(path), self.pathwidth)

    def processFile(self, path):
        if os.path.splitext(path)[1].lower() != '.chd':
            with open(path, mode='rb', buffering=0) as f:
                self.processRomFile(path, f)
        else:
            with open(path, mode='rb') as f:
                sha1 = self.probeChd(f)
                if sha1 is None:
                    f.seek(0)
                    self.processRomFile(path, f)
                else:
                    self.processChd(path, sha1)

    def processPath(self, path, depth=0):
        try:
            if not os.path.isdir(path):
                self.processFile(path)
            elif depth > 5:
                sys.stderr.write('Not examining \'%s\' - maximum depth exceeded\n')
            else:
                for name in os.listdir(path):
                    self.processPath(os.path.join(path, name), depth + 1)
        except BaseException as e:
            sys.stderr.write('Error identifying \'%s\': %s\n' % (path, e))

    def printResults(self):
        pw = self.pathwidth - (self.pathwidth % 4) + 4
        lw = self.labelwidth - (self.labelwidth % 4) + 4
        first = True
        for shortname, romset in sorted(self.matches.items()):
            if first:
                first = False
            else:
                sys.stdout.write('\n')
            sys.stdout.write('%-20s%s\n' % (shortname, romset[0]))
            for path, label, bad in romset[1]:
                if bad:
                    sys.stdout.write('    %-*s= %-*s(BAD)\n' % (pw, path, lw, label))
                else:
                    sys.stdout.write('    %-*s= %s\n' % (pw, path, label))
        if self.unmatched:
            if first:
                first = False
            else:
                sys.stdout.write('\n')
            sys.stdout.write('Unmatched\n')
            for path, crc, sha1 in self.unmatched:
                if crc is not None:
                    sys.stdout.write('    %-*sCRC(%08x) SHA1(%s)\n' % (pw, path, crc, sha1))
                else:
                    sys.stdout.write('    %-*sSHA1(%s)\n' % (pw, path, sha1))

    @staticmethod
    def iterateBlocks(f, s=65536):
        while True:
            buf = f.read(s)
            if buf:
                yield buf
            else:
                break

    @staticmethod
    def digestRom(f):
        crc = zlib.crc32(bytes())
        sha = hashlib.sha1()
        for block in _Identifier.iterateBlocks(f):
            crc = zlib.crc32(block, crc)
            sha.update(block)
        return crc & 0xffffffff, sha.hexdigest()

    @staticmethod
    def probeChd(f):
        buf = f.read(16)
        if (len(buf) != 16) or (buf[:8] != b'MComprHD'):
            return None
        headerlen, version = struct.unpack('>II', buf[8:])
        if version == 3:
            if headerlen != 120:
                return None
            sha1offs = 80
        elif version == 4:
            if headerlen != 108:
                return None
            sha1offs = 48
        elif version == 5:
            if headerlen != 124:
                return None
            sha1offs = 84
        else:
            return None
        f.seek(sha1offs)
        if f.tell() != sha1offs:
            return None
        buf = f.read(20)
        if len(buf) != 20:
            return None
        return codecs.getencoder('hex_codec')(buf)[0].decode('ascii')


def do_listfull(options):
    dbconn = dbaccess.QueryConnection(options.database)
    dbcurs = dbconn.cursor()
    first = True
    for shortname, description in dbcurs.listfull(options.pattern):
        if first:
            sys.stdout.write('Name:             Description:\n')
            first = False
        sys.stdout.write('%-16s  "%s"\n' % (shortname, description))
    if first:
        sys.stderr.write('No matching systems found for \'%s\'\n' % (options.pattern, ))
    dbcurs.close()
    dbconn.close()


def do_listsource(options):
    dbconn = dbaccess.QueryConnection(options.database)
    dbcurs = dbconn.cursor()
    shortname = None
    for shortname, sourcefile in dbcurs.listsource(options.pattern):
        sys.stdout.write('%-16s %s\n' % (shortname, sourcefile))
    if shortname is None:
        sys.stderr.write('No matching systems found for \'%s\'\n' % (options.pattern, ))
    dbcurs.close()
    dbconn.close()


def do_listclones(options):
    dbconn = dbaccess.QueryConnection(options.database)
    dbcurs = dbconn.cursor()
    first = True
    for shortname, parent in dbcurs.listclones(options.pattern):
        if first:
            sys.stdout.write('Name:            Clone of:\n')
            first = False
        sys.stdout.write('%-16s %s\n' % (shortname, parent))
    if first:
        count = dbcurs.count_systems(options.pattern).fetchone()[0]
        if count:
            sys.stderr.write('Found %d match(es) for \'%s\' but none were clones\n' % (count, options.pattern))
        else:
            sys.stderr.write('No matching systems found for \'%s\'\n' % (options.pattern, ))
    dbcurs.close()
    dbconn.close()


def do_listbrothers(options):
    dbconn = dbaccess.QueryConnection(options.database)
    dbcurs = dbconn.cursor()
    first = True
    for sourcefile, shortname, parent in dbcurs.listbrothers(options.pattern):
        if first:
            sys.stdout.write('%-20s %-16s %s\n' % ('Source file:', 'Name:', 'Parent:'))
            first = False
        sys.stdout.write('%-20s %-16s %s\n' % (sourcefile, shortname, parent or ''))
    if first:
        sys.stderr.write('No matching systems found for \'%s\'\n' % (options.pattern, ))
    dbcurs.close()
    dbconn.close()


def do_listaffected(options):
    dbconn = dbaccess.QueryConnection(options.database)
    dbcurs = dbconn.cursor()
    first = True
    for shortname, description in dbcurs.listaffected(*options.pattern):
        if first:
            sys.stdout.write('Name:             Description:\n')
            first = False
        sys.stdout.write('%-16s  "%s"\n' % (shortname, description))
    if first:
        sys.stderr.write('No matching systems found for \'%s\'\n' % (options.pattern, ))
    dbcurs.close()
    dbconn.close()


def do_romident(options):
    dbconn = dbaccess.QueryConnection(options.database)
    dbcurs = dbconn.cursor()
    ident = _Identifier(dbcurs)
    for path in options.path:
        ident.processPath(path)
    ident.printResults()
    dbcurs.close()
    dbconn.close()
