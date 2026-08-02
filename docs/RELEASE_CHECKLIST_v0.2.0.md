# KSJ_Storage v0.2.0 Release Checklist

## Code

- [ ] `library.json` reports `0.2.0`
- [ ] `git diff --check` reports no whitespace errors
- [ ] Example compiles against the local symlinked library
- [ ] Upload succeeds on ESP32 DOIT DevKit V1
- [ ] Invalid payload is rejected
- [ ] Sequence is preserved after rejection
- [ ] Previous session reports `COMPLETE`

## Retention validation

Temporarily set:

```cpp
policy.maximumSessions = 5;
```

- [ ] Boot with more than five matching sessions already on the card
- [ ] Retention reports deleted sessions
- [ ] Only the newest complete sessions remain
- [ ] A new active session is created afterward
- [ ] The card contains five matching sessions after the new session begins
- [ ] Incomplete sessions, if any, are not deleted

Restore:

```cpp
policy.maximumSessions = 100;
```

- [ ] Rebuild and upload the release configuration
- [ ] Storage health reports `HEALTHY`

## Documentation

- [ ] README reflects v0.2.0
- [ ] CHANGELOG contains v0.2.0
- [ ] Release notes are ready
- [ ] Development workflow document remains linked

## Git

- [ ] Commit 4 created
- [ ] Working tree is clean
- [ ] Four focused v0.2.0 commits are visible in history
- [ ] Annotated tag `v0.2.0` created
- [ ] Branch pushed
- [ ] Tag pushed
- [ ] GitHub release created from the release notes
