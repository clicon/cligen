Built with `git-buildpackage`.

## Building a deb package

1. Create a tag in *upstream-branch*

```
git tag <version>
```

2. Switch to *debian-branch* and import tag

```
git switch <debian-branch>
gbp import-ref -u <version>
```

3. Update the *debian/changelog* file and commit it, eg

```
gbp dch -N <version>-<debian revision>
git add debian/
git commit
```

4. (If needed) Create pbuilder base

```
DIST=<distribution> git-pbuilder create
```

**Note**: If building for a Debian distribution on an Ubuntu machine, use

```
DIST=<distribution> git-pbuilder create \
  --mirror http://deb.debian.org/debian/ \
  --debootstrapopts --keyring=/usr/share/keyrings/debian-archive-keyring.gpg
```

5. Build package

```
gbp buildpackage --git-dist=<distribution>
```

**Note**: Built package ends up in directory `export-dir` specified in **debian/gbp.conf**.
