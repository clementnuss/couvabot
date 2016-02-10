To generate a list of the files to be removed, use :

  find -follow > filelist

Then to effectively delete files (and folders) listed in that list, use :

  xargs -a filelist -d'\n' rm -r
