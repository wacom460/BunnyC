## IBLayer3 compiler
prefix based language experiment
### Hello world example:
    blk $main i32 $argc nts^ $argv -> i32
        if $argc >= 0
            "Hello, %% %%" $argc "world!"
        _
    _ 0
