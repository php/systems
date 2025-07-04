This is a very simple script to dump the members of the PHP GitHub
organization in Markdown format.

It requires a GitHub personal access token (classic) with `read:org`
and `admin:org` privileges.

To run:

```sh
$ export GITHUB_TOKEN="..."
$ /local/systems/dump-gh-org/dump-gh-org > out.md
```
