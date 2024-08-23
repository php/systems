This is a very simple script to dump the members of the PHP GitHub
organization in DokuWiki format.

It requires a GitHub personal access token (classic) with `read:org`
privileges.

To run:

```sh
$ export GITHUB_TOKEN="..."
$ /local/systems/dump-gh-org/dump-gh-org > out.wiki
```
