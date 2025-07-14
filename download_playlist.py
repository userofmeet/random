import yt_dlp
def download_audio_from_playlist(playlist_url):
    ydl_opts = {
        'format': 'bestaudio/best',
        'outtmpl': '%(title)s.%(ext)s',
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

playlist_link = "https://youtube.com/playlist?list=PLIsj2dWOZHsBk0NrHJmwn10HI6Y2hutui&feature=shared"
download_audio_from_playlist(playlist_link)
