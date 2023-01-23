
function Format-Files($folder, $skipFolders, $depth = 5){
    cd $folder
    Get-ChildItem -Depth 0 -File | ForEach-Object {
        clang-format $_.Name
    }
    if($depth -gt 0){
        Get-ChildItem -Depth 0 -Directory | ForEach-Object {
            if($_.Name -notin $skipFolders){
                Format-Files $_.Name $depth-1
            }
        }
    }
    cd..    
}
Format-Files "include" @("lvgl")
Format-Files "src" @()
Format-Files "Libs" @("Eigen")
