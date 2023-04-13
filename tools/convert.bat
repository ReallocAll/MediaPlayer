ffmpeg.exe -i input.mp4 -r 20 -vf scale=128:-1 -b:v 128k -c:v libx264 -preset medium -c:a aac output.mp4
ffmpeg.exe -i output.mp4 -vf fps=20,scale=128:-1:flags=lanczos,pad=128:128:(ow-iw)/2:(oh-ih)/2:color=black,format=rgba output/%04d.png
pause
