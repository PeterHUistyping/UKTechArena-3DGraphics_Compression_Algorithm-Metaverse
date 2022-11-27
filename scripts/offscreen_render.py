
import argparse
import numpy as np
import trimesh
import os
from pyglet import gl
from pyvirtualdisplay import Display

if __name__ == '__main__':
    # Read and parse arguments from command line
    parser = argparse.ArgumentParser()

    parser.add_argument("-i", "--Input", help = "Input GLTF/Obj File")
    parser.add_argument("-o", "--Output", help = "Output PNG File")

    args = parser.parse_args()

    # print logged messages
    # trimesh.util.attach_to_log()

    # Comment this out to use native display
    display = Display(visible=0, size=(1920, 1080))
    display.start()

    if args.Input:
        # print("Displaying Input as: % s" % args.Input)
        mesh = trimesh.load(args.Input, force='mesh')
        scene = mesh.scene()
        # mesh.show(flags={'wireframe': True})

        window_conf = gl.Config(double_buffer=True, depth_size=24)

        # a 45 degree homogeneous rotation matrix around
        # the Y axis at the scene centroid
        rotate = trimesh.transformations.rotation_matrix(
            angle=np.radians(10.0),
            direction=[0, 1, 0],
            point=scene.centroid)

        # rotate the camera view transform
        camera_old, _geometry = scene.graph[scene.camera.name]
        camera_new = np.dot(rotate, camera_old)

        # apply the new transform
        scene.graph[scene.camera.name] = camera_new

        # saving an image requires an opengl context, so if -nw
        # is passed don't save the image
        try:
            if args.Output:
                # print("Displaying Output as: % s" % args.Output)
                # save a render of the object as a png
                png = scene.save_image(resolution=[1920, 1080], window_conf=window_conf)
                os.makedirs(os.path.dirname(args.Output), exist_ok=True)
                with open(args.Output, 'wb') as f:
                    f.write(png)
                    f.close()
        except BaseException as E:
            print("unable to save image", str(E))
