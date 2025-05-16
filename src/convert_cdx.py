import json
from collections import defaultdict
from urllib.parse import urlparse
import requests

# Get the CDX data
url = "https://web.archive.org/cdx/search/cdx?url=dev-www.snipeoffice.org/themes/*&output=json&fl=timestamp,original&filter=statuscode:200"
response = requests.get(url)
data = response.json()

# Skip the header
rows = data[1:]

# Find the latest snapshot for each file
latest = {}
for timestamp, url in rows:
    # Only consider actual files, not directory listings
    path = urlparse(url).path
    if path.endswith('/src/') or path.endswith('/src//'):
        continue
    filename = path.split('/')[-1]
    if filename not in latest or timestamp > latest[filename][0]:
        latest[filename] = (timestamp, url)

# Build the download URLs
with open('wayback_download_urls.txt', 'w') as out:
    for filename, (timestamp, url) in latest.items():
        # Construct the Wayback Machine direct download URL
        archive_url = f"https://web.archive.org/web/{timestamp}if_/{url}"
        out.write(archive_url + '\n')

print("wayback_download_urls.txt created with latest snapshots for each file.") 