import os
import yt_dlp

def download_audio_from_playlist(playlist_url):
    download_path = os.path.join(os.path.expanduser("~"), "Downloads", "Songs Downloaded")
    os.makedirs(download_path, exist_ok=True)

    ydl_opts = {
        'format': 'bestaudio/best',
        'outtmpl': os.path.join(download_path, '%(title)s.%(ext)s'),  
        'postprocessors': [{
            'key': 'FFmpegExtractAudio',
            'preferredcodec': 'mp3',
            'preferredquality': '192',
        }],
        'noplaylist': False,
        'ignoreerrors': True,
        'quiet': False
    }

    with yt_dlp.YoutubeDL(ydl_opts) as ydl:
        ydl.download([playlist_url])

playlist_link = "https://youtube.com/playlist?list=PLIsj2dWOZHsAEzoWYIH5w1-jWkM7soCwU&feature=shared"
download_audio_from_playlist(playlist_link)
