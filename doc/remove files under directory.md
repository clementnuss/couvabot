###Delete files in a specific folder of another folder

We achieve that in 2 steps :

Firstly we generate a list of the files to be removed, using :

```find -follow > filelist ```

Then to effectively delete files (and folders) listed in that list, use :

```xargs -a filelist -d'\n' rm -r
```
