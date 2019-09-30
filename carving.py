import argparse
import numpy as np
import imageio
import matplotlib.pyplot as plt
from matplotlib import animation
from scipy.signal import convolve2d

parser = argparse.ArgumentParser(description="Script to perform scene carving on an image")
parser.add_argument("file", type=str, help="Path to image to be carved")
parser.add_argument("-m", type=int, default=10, help="Margin for each cut")

args = parser.parse_args()

image = imageio.imread(args.file).astype(np.float) / 256.0

fig = plt.figure()
im = plt.imshow(image, interpolation='none')

def rgb2gray(rgb):
    r, g, b = rgb[:,:,0], rgb[:,:,1], rgb[:,:,2]
    gray = 0.2989 * r + 0.5870 * g + 0.1140 * b

    return gray

def carve(image, margin=5, mode='contract'):    
    if image.ndim == 3:
        gray = rgb2gray(image)
    else:
        gray = image

    gradx = convolve2d(gray, np.array([[-1, 0, 1]]), mode='same')
    grady = convolve2d(gray, np.array([[-1, 0, 1]]).T, mode='same')

    cost = np.sqrt(gradx ** 2 + grady ** 2)

    path = np.zeros_like(cost)
    path[0] = cost[0]

    for row in range(1, cost.shape[0]):
        for col in range(cost.shape[1]):
            if col == 0:
                path[row, col] = cost[row, col] + min(path[row - 1, col], path[row - 1, col + 1])

            elif col == cost.shape[1] - 1:
                path[row, col] = cost[row, col] + min(path[row - 1, col], path[row - 1, col - 1])

            else:
                path[row, col] = cost[row, col] + min(path[row - 1, col + 1], path[row - 1, col], path[row - 1, col - 1])

    row = path.shape[0] - 1
    index = np.argmin(path[row])

    if mode == 'expand':
        out = np.zeros((image.shape[0], image.shape[1] + margin, image.shape[2]))
        out[:,:image.shape[1]] = image
    else:
        out = image.copy()

    while row >= 0:
        if index == 0:
            index = np.argmin(path[row, 0 : 2])
        elif index == image.shape[1] - 1:
            index = index + np.argmin(path[row, image.shape[1] - 2: image.shape[1]]) - 1
        else:
            index = index + np.argmin(path[row, index - 1 : index + 2]) - 1

        if mode == 'contract':
            out[row, index:out.shape[1] - margin] = image[row, index + margin:out.shape[1]]
        elif mode == 'expand':
            out[row, index + margin + 1:] = image[row, index + 1:]
            out[row, index:index + margin + 1] = np.linspace(image[row, index], image[row, index + 1], margin + 1)
        
        row -= 1

    if mode == 'expand':
        return out
    elif mode == 'contract':
        return out[:, :-margin]


plt.ion()

margin = args.m
print("Performing image carving on {} with margin {}".format(args.file, margin))

while image.shape[1] > margin:
    user = input("Press [enter] to continue, or q to exit: ")

    if user == 'q':
        break

    print(image.shape)

    image = carve(image, margin, mode='contract')
    plt.imshow(image)
    plt.draw()
    plt.pause(0.001)
    
# def init():
#     # im.set_data(im.get_array().astype(np.float) / 256.0)
#     return [im]

# def animate(i):
#     a = im.get_array()
#     a = a*np.exp(-0.001*i)    # exponential decay of the values
#     im.set_array(a)
#     return [im]

# anim = animation.FuncAnimation(fig, animate, init_func=init,
#                                frames=50, interval=20, blit=True)

# plt.show()
