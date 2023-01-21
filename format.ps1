cd include
Get-ChildItem -Depth 0 -Directory | ForEach-Object {
    
    cd $_.Name
    if($_.Name -ne "lvgl"){
        Get-ChildItem -Depth 0 -File | ForEach-Object {
            # echo $_.Name

            clang-format $_.Name
        }
        Get-ChildItem -Depth 0 -Directory | ForEach-Object {
        
            cd $_.Name
            Get-ChildItem -Depth 0 -File | ForEach-Object {
                # echo $_.Name

                clang-format $_.Name
            }
            
            cd..
            # clang-format $_.Name
        }
    }
    cd..
    # clang-format $_.Name
}
cd..

cd src
Get-ChildItem -Depth 0 -Directory | ForEach-Object {
    
    cd $_.Name
    if($_.Name -ne "lvgl"){
        Get-ChildItem -Depth 0 -File | ForEach-Object {
            # echo $_.Name

            clang-format $_.Name
        }
        Get-ChildItem -Depth 0 -Directory | ForEach-Object {
        
            cd $_.Name
            Get-ChildItem -Depth 0 -File | ForEach-Object {
                # echo $_.Name

                clang-format $_.Name
            }
            
            cd..
            # clang-format $_.Name
        }
    }
    cd..
    # clang-format $_.Name
}
cd..
cd Libs
Get-ChildItem -Depth 0 -Directory | ForEach-Object {
    
    cd $_.Name
    if($_.Name -ne "Eigen"){
        Get-ChildItem -Depth 0 -File | ForEach-Object {
            # echo $_.Name

            clang-format $_.Name
        }
        Get-ChildItem -Depth 0 -Directory | ForEach-Object {
        
            cd $_.Name
            Get-ChildItem -Depth 0 -File | ForEach-Object {
                # echo $_.Name

                clang-format $_.Name
            }
            
            cd..
            # clang-format $_.Name
        }
    }
    cd..
    # clang-format $_.Name
}
cd..

