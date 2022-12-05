from flask import Flask, render_template, Response
import autoaim
import argparse

app = Flask(__name__)


"""
def gen_frames():
    # camera = cv2.VideoCapture(-1)
    while True:
        frame = webvideo.get_img()  # read the camera frame
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')  # concat frame one by one and show result
"""
@app.route("/")
def index():
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    return Response(autoaim.get_cvmat_jpegcode(), mimetype='multipart/x-mixed-replace;boundary=frame')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--script", "-s", nargs='+', default=[], help="startup script(s) before start the web app")
    opt = parser.parse_args()
    for script in opt.script:
        print(f"running startup script: '{script}'")
        exec(open(script).read())
    app.run(host="0.0.0.0", port=5000, threaded=True)