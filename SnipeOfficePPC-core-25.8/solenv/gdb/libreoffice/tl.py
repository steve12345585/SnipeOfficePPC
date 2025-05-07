# -*- tab-width: 4; indent-tabs-mode: nil; py-indent-offset: 4 -*-
# Version: MPL 1.1 / GPLv3+ / LGPLv3+
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License or as specified alternatively below. You may obtain a copy of
# the License at http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# Major Contributor(s):
# Copyright (C) 2010 Red Hat, Inc., David Tardon <dtardon@redhat.com>
#  (initial developer)
#
# All Rights Reserved.
#
# For minor contributions see the git repository.
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 3 or later (the "GPLv3+"), or
# the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
# in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
# instead of those above.

import gdb

from libreoffice.util import printing
from libreoffice.util.string import StringPrinterHelper

class StringPrinter(StringPrinterHelper):
    '''Prints ByteString or UniString'''

    def __init__(self, typename, val, encoding = None):
        super(StringPrinter, self).__init__(typename, val, encoding)

    def valid(self):
        data = self.val['mpData']
        # mnRefCount is not a good indicator: it seems there could be
        # cases where it is negative (-7FFFFFED)
        return data #and data.dereference()['mnRefCount'] > 0

    def data(self):
        assert self.val['mpData']
        return self.val['mpData'].dereference()['maStr']

    def length(self):
        assert self.val['mpData']
        return self.val['mpData'].dereference()['mnLen']

class BigIntPrinter(object):
    '''Prints big integer'''

    def __init__(self, typename, val):
        self.val = val

    def to_string(self):
        if self.val['bIsSet']:
            if self.val['bIsBig']:
                return self._value()
            else:
                return self.val['nVal']
        else:
            return "unset %s" % self.typename

    def _value(self):
        len = self.val['nLen']
        digits = self.val['nNum']
        dsize = digits.dereference().type.sizeof * 8
        num = 0
        # The least significant byte is on index 0
        for i in reversed(range(0, len)):
            num <<= dsize
            num += digits[i]
        return num

class ColorPrinter(object):
    '''Prints color as rgb(r, g, b) or rgba(r, g, b, a)'''

    def __init__(self, typename, val):
        self.val = val

    def to_string(self):
        color = self.val['mnColor']
        b = color & 0xff
        g = (color >> 8) & 0xff
        r = (color >> 16) & 0xff
        a = (color >> 24) & 0xff
        if a:
            return "rgba(%d, %d, %d, %d)" % (r, g, b, a)
        else:
            return "rgb(%d, %d, %d)" % (r, g, b)

class FractionPrinter(object):
    '''Prints fraction'''

    def __init__(self, typename, val):
        self.typename = typename
        self.val = val

    def to_string(self):
        numerator = self.val['nNumerator']
        denominator = self.val['nDenominator']
        if denominator > 0:
            return "%d/%d" % (numerator, denominator)
        else:
            return "invalid %s" % self.typename

class DateTimeImpl(object):

    def __init__(self, date, time):
        self.date = date
        self.time = time

    def __str__(self):
        result = ''
        if self.date:
            result += str(self.date)
            if self.time:
                result += ' '
        if self.time:
            result += str(self.time)
        return result

    @staticmethod
    def parse(val):
        return DateTimeImpl(DateImpl.parse(val), TimeImpl.parse(val))

class DateTimePrinter(object):
    '''Prints date and time'''

    def __init__(self, typename, val):
        self.val = val

    def to_string(self):
        return str(DateTimeImpl.parse(self.val))

class DateImpl(DateTimeImpl):

    def __init__(self, year, month, day):
        super(DateImpl, self).__init__(self, None)
        self.year = year
        self.month = month
        self.day = day

    def __str__(self):
        return "%d-%d-%d" % (self.year, self.month, self.day)

    @staticmethod
    def parse(val):
        date = val['nDate']
        d = date % 100
        m = (date / 100) % 100
        y = date / 10000
        return DateImpl(y, m, d)

class DatePrinter(object):
    '''Prints date'''

    def __init__(self, typename, val):
        self.val = val

    def to_string(self):
        return str(DateImpl.parse(self.val))

class TimeImpl(DateTimeImpl):

    def __init__(self, hour, minute, second, hundreth_of_second = 0):
        super(TimeImpl, self).__init__(None, self)
        self.hour = hour
        self.minute = minute
        self.second = second
        self.hundreth_of_second = hundreth_of_second

    def __str__(self):
        decimal = ''
        if self.hundreth_of_second != 0:
            decimal = '.%d' % self.hundreth_of_second
        return "%d:%d:%d%s" % (self.hour, self.minute, self.second, decimal)

    @staticmethod
    def parse(val):
        time = val['nTime']
        h = time / 1000000
        m = (time / 10000) % 100
        s = (time / 100) % 100
        s_100 = time % 100
        return TimeImpl(h, m, s, s_100)

class TimePrinter(object):
    '''Prints time'''

    def __init__(self, typename, val):
        self.val = val

    def to_string(self):
        return str(TimeImpl.parse(self.val))

class IteratorHelper(object):
    '''Implements a container iterator useable for both 'linear'
        containers (like DynArray or List) and Tables
    '''

    def __init__(self, block, count, type = None):
        self.count = count
        self.type = type
        self.pos = 0
        self.block = None
        self.block_count = 0
        self.block_pos = 0
        if block:
            self._next_block(block)

        self._check_invariant()

    def __iter__(self):
        return self

    def next(self):
        if self.pos == self.count:
            raise StopIteration()

        if self.block_pos == self.block_count:
            self._next_block(self.block['pNext'])

        name = self.name()
        val = self.value()
        self.advance()

        self._check_invariant()
        return (name, val)

    def _next_block(self, block):
        assert block

        self.block = block.dereference()
        self.block_pos = 0
        self.block_count = block['nCount']

        assert self.block_count <= block['nSize']
        assert self.block_count + self.pos <= self.count

    def _check_invariant(self):
        assert self.count >= 0
        assert self.pos >= 0
        assert self.pos <= self.count
        assert self.block_count >= 0
        if self.pos < self.count:
            assert self.block_count > 0
            assert self.block != None
        assert self.block_count <= self.count
        assert self.block_pos >= 0
        assert self.block_pos <= self.block_count

class NoItemType(Exception):
    pass

class ContainerHelper(object):
    '''Provides support for specialized container printers'''

    def __init__(self, typename, val, iterator):
        self.typename = typename
        self.val = val
        self.iterator = iterator

    def to_string(self):
        size = self.val['nCount']
        if size > 0:
            return "%s of length %d" % (self.typename, size)
        elif size == 0:
            return "empty %s" % self.typename
        else:
            return "invalid %s" % self.typename

    def children(self):
        count = self.val.cast(gdb.lookup_type('Container'))['nCount']
        return self.iterator(self.val['pFirstBlock'], count)

class LinearIterator(IteratorHelper):
    '''Is iterator for 'linear' container'''

    def __init__(self, block, count, type = None):
        super(LinearIterator, self).__init__(block, count, type)

    def name(self):
        return str(self.pos)

    def value(self):
        nodes = self.block['pNodes']#.cast(self.type.pointer())
        return nodes[self.block_pos]

    def advance(self):
        self.pos += 1
        self.block_pos += 1

class LinearContainerPrinter(ContainerHelper):
    '''Prints 'linear' container, like DynArray or List'''

    def __init__(self, typename, val):
        super(LinearContainerPrinter, self).__init__(typename, val, LinearIterator)

    def display_hint(self):
        return 'array'

class TableIterator(IteratorHelper):
    '''Is iterator for Table'''

    def __init__(self, block, count, type = None):
        super(TableIterator, self).__init__(block, count, type)
        # ULONG doesn't work on 64-bit for some reason (gdb says it has
        # size 4 and it's not a typedef to sal_uIntPtr)
        self._key_type = gdb.lookup_type('sal_uIntPtr')
        self.is_key = True

    def name(self):
        return ''

    def value(self):
        nodes = self.block['pNodes']#.cast(self.type.pointer())
        val = nodes[self.block_pos]
        if self.is_key:
            val = str(val.cast(self._key_type))
        return val

    def advance(self):
        self.pos += 1
        self.block_pos += 1
        self.is_key = not self.is_key

class TablePrinter(ContainerHelper):
    '''Prints table'''

    def __init__(self, typename, val):
        super(TablePrinter, self).__init__(typename, val, TableIterator)

    def display_hint(self):
        return 'map'

printer = None

def build_pretty_printers():
    global printer

    printer = printing.Printer('libreoffice/tl')

    # old-style strings
    printer.add('ByteString', StringPrinter)
    printer.add('String', lambda name, val: StringPrinter(name, val, 'utf-16'))

    # old-style containers
    printer.add('DynArray', LinearContainerPrinter)
    printer.add('List', LinearContainerPrinter)
    printer.add('Stack', LinearContainerPrinter)
    printer.add('Table', TablePrinter)

    # various types
    printer.add('BigInt', BigIntPrinter)
    printer.add('Color', ColorPrinter)
    printer.add('Fraction', FractionPrinter)
    printer.add('DateTime', DateTimePrinter)
    printer.add('Date', DatePrinter)
    printer.add('Time', TimePrinter)

def register_pretty_printers(obj):
    printing.register_pretty_printer(printer, obj)

build_pretty_printers()

# vim:set shiftwidth=4 softtabstop=4 expandtab:
