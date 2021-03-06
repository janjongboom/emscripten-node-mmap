var mmap = require('./build/Release/mmap');
var fs = require('fs');

var fd = fs.openSync(__dirname + '/test.js', 'r');

var map = mmap.mmap(8, mmap.PROT_READ, mmap.MAP_SHARED, fd, 0);
console.log(map);
console.log('map address is', typeof map.ptr, map.ptr);
console.log('unmap return value is', mmap.munmap(map.ptr, 8));

fs.closeSync(fd);
