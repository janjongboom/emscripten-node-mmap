var mmap = require('./build/Release/mmap');
var fs = require('fs');

var fd = fs.openSync('./index.js', 'r');

console.log('fd', fd);

var map = mmap.mmap(8, mmap.PROT_READ, mmap.MAP_SHARED, fd, 0);
console.log('map address is', map);
console.log('unmap return value is', mmap.munmap(map, 10));

fs.closeSync(fd);
