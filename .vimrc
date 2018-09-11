set ts=4
set sts=4
set sw=4
set expandtab

set colorcolumn=80
highlight ColorColumn guibg=#2e2e37

augroup project
    autocmd!
    autocmd BufRead,BufNewFile *.h,*.c set filetype=c
augroup END
