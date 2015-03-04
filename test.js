var mmap = require('./build/Release/mmap');
var fs = require('fs');

var fd = fs.openSync('./package.json', 'r');

var map = mmap.mmap(4, mmap.PROT_READ, mmap.MAP_SHARED, fd, 0);
console.log('map address is', map);
console.log('unmap return value is', mmap.munmap(map, 10));

fs.closeSync(fd);
