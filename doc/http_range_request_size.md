# HTTP `range_request_size`

This fork adds an HTTP input option named `range_request_size` in
`libavformat/http.c`.

## Purpose

Some HTTP download endpoints perform better when the client reads the media as
multiple bounded `Range` requests instead of one long-lived request. This
option lets FFmpeg split one logical input stream into sequential HTTP range
windows such as:

```text
bytes=0-5242879
bytes=5242880-10485759
bytes=10485760-15728639
...
```

The demuxer still sees one continuous byte stream. The split only happens in
the HTTP protocol layer.

## Option

- Name: `range_request_size`
- Scope: HTTP input
- Type: `int64`
- Unit: bytes
- Default: `0` meaning disabled

When the value is greater than `0`, FFmpeg limits each HTTP GET request to at
most that many bytes and transparently reopens the next request window after
the previous one finishes.

## Behavior

- `end_offset` keeps its original meaning as the logical exclusive end offset.
- The actual request window is:

```text
min(end_offset, current_offset + range_request_size)
```

- If `headers` already contains a custom `Range:` header, that header wins and
  `range_request_size` does not override it.
- This option is intended for input. It does not add parallel downloads.
- `multiple_requests=1` is recommended so reconnecting to the next window can
  reuse persistent connections when the server allows it.

## Command-line usage

Basic example:

```bash
ffplay -seekable 1 -multiple_requests 1 \
  -range_request_size 5242880 \
  INPUT_URL
```

With custom headers:

```bash
ffplay -seekable 1 -multiple_requests 1 \
  -range_request_size 5242880 \
  -headers $'Cookie: ...\r\nReferer: https://pan.quark.cn/\r\n' \
  INPUT_URL
```

For protocol-private options such as redirect or TLS verification policy, use
`-protocol_opts`:

```bash
ffprobe -seekable 1 -multiple_requests 1 \
  -protocol_opts follow_redirect=1:verify_ssl=0 \
  -range_request_size 5242880 \
  INPUT_URL
```

## Library usage

Pass the option through `AVDictionary` when opening the input:

```c
AVDictionary *opts = NULL;
av_dict_set(&opts, "multiple_requests", "1", 0);
av_dict_set(&opts, "range_request_size", "5242880", 0);
avformat_open_input(&fmt, url, NULL, &opts);
```

If the application opens the protocol layer directly, `avio_open2()` can use
the same option:

```c
AVDictionary *opts = NULL;
av_dict_set(&opts, "multiple_requests", "1", 0);
av_dict_set(&opts, "range_request_size", "5242880", 0);
avio_open2(&pb, url, AVIO_FLAG_READ, NULL, &opts);
```

## Verification

Use trace logging and inspect the generated request headers:

```bash
ffprobe -loglevel trace -seekable 1 -multiple_requests 1 \
  -range_request_size 5242880 INPUT_URL
```

Expected log pattern:

```text
Range: bytes=0-5242879
Range: bytes=5242880-10485759
Range: bytes=10485760-15728639
```

## Limits

- This option does not force the remote service to accept the request.
- If the server rejects the signed URL, cookies, or headers, FFmpeg can still
  fail with `403` or `412`.
- This option improves request shape control, not authentication refresh.
