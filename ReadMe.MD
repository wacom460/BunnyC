## IBLayer3 compiler
prefix based language experiment
### Hello world example:
    @blk $main %i32 $argc %^^c8 $argv -> %i32
        "Hello, %%" "world!"
        "Argument count: %%" $argc
    _ 0
