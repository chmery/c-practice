## homemade txt compressor

works only with ASCII characters

### encoded file structure

- 1B - huffman tree nodes count
- nB - n bytes of nodes stored as array using depth-first approach
- 4B - data size
- 1B - last data byte's padding bits count (irrelevant bits to skip)
- nB - n bytes of encoded data
