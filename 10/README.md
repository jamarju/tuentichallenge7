There were two gotchas in this one, first one was realizing that this loop was a modular exponentiation:

```php
for ($i=0; $i<10000000; $i++) {
  # This loop makes the passwords hard to reverse
  $counter = ($counter * $secret1) % $secret2;
}
```

And most languages have efficient modular exponentiation facilities:

https://rosettacode.org/wiki/Modular_exponentiation

For example, in Python, the above loop translates to:

```python
counter = s3 * pow(s1, 10000000, s2) % s2
```

The second gotcha was that a few commits were deleted and don't appear on a `git log`. You can still check them out, you just need their SHA1s:

```
git fsck --unreachable |grep commit |cut -f 3 -d ' '
```
