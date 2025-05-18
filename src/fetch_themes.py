import requests
import json
import os
from bs4 import BeautifulSoup
from urllib.parse import urljoin
import time

def get_wayback_snapshots(url):
    """Get all available snapshots for a URL from the Wayback Machine."""
    cdx_url = f"https://web.archive.org/cdx/search/cdx?url={url}&output=json"
    response = requests.get(cdx_url)
    if response.status_code != 200:
        print(f"Error fetching snapshots: {response.status_code}")
        return []
    
    data = response.json()
    if len(data) < 2:  # First row is headers
        return []
    
    snapshots = []
    for row in data[1:]:  # Skip header row
        timestamp = row[1]
        original_url = row[2]
        snapshots.append({
            'timestamp': timestamp,
            'url': original_url
        })
    return snapshots

def download_theme_file(wayback_url, output_dir):
    """Download a theme file from the Wayback Machine."""
    try:
        response = requests.get(wayback_url)
        if response.status_code != 200:
            print(f"Error downloading {wayback_url}: {response.status_code}")
            return False
        
        # Extract filename from URL
        filename = wayback_url.split('/')[-1]
        if not filename:
            print(f"Could not determine filename for {wayback_url}")
            return False
        
        output_path = os.path.join(output_dir, filename)
        with open(output_path, 'wb') as f:
            f.write(response.content)
        print(f"Successfully downloaded {filename}")
        return True
    except Exception as e:
        print(f"Error downloading {wayback_url}: {str(e)}")
        return False

def main():
    # Create themes directory if it doesn't exist
    themes_dir = "themes"
    os.makedirs(themes_dir, exist_ok=True)
    
    # Base URL for SnipeOffice themes
    base_url = "https://dev-www.snipeoffice.org/themes"
    
    # Get snapshots
    print("Fetching available snapshots...")
    snapshots = get_wayback_snapshots(base_url)
    
    if not snapshots:
        print("No snapshots found!")
        return
    
    # Get the most recent snapshot
    latest_snapshot = snapshots[0]
    timestamp = latest_snapshot['timestamp']
    wayback_url = f"https://web.archive.org/web/{timestamp}/{base_url}"
    
    print(f"Using snapshot from {timestamp}")
    
    # Fetch the page content
    response = requests.get(wayback_url)
    if response.status_code != 200:
        print(f"Error fetching page: {response.status_code}")
        return
    
    # Parse the page
    soup = BeautifulSoup(response.text, 'html.parser')
    
    # Find all theme links (adjust selectors based on actual page structure)
    theme_links = soup.find_all('a', href=True)
    
    # Download each theme file
    for link in theme_links:
        href = link['href']
        if href.endswith(('.zip', '.tar.gz', '.tgz')):
            full_url = urljoin(wayback_url, href)
            wayback_full_url = f"https://web.archive.org/web/{timestamp}/{full_url}"
            print(f"Downloading theme: {href}")
            download_theme_file(wayback_full_url, themes_dir)
            time.sleep(1)  # Be nice to the Wayback Machine

if __name__ == "__main__":
    main() 