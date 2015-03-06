{
  "targets": [
    {
      "target_name": "mmap",
      "include_dirs": ["<!(node -p -e \"require('path').dirname(require.resolve('nan'))\")"],
      "sources": [ "mmap.cc" ]
    }
  ]
}