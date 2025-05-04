import numpy as np
from PIL import Image
import os
import time
from concurrent.futures import ThreadPoolExecutor


def compute_channel(image_array, channel, result):
    result[:, :, channel] = image_array[:, :, channel].cumsum(axis=0).cumsum(axis=1) #numpy per calcolare velocemente l'integrale di ogni canale RGB
    
def integralImage(image_array, imgnum):
    result = np.zeros_like(image_array, dtype=np.uint64)
    for i in range(3):
        compute_channel(image_array, i, result)

    print("Integral image per canale RGB dell'immagine ", imgnum, " completato")
    #print("R: \n",result[:,:,0])
    #print("G: \n",result[:,:,1])
    #print("B: \n",result[:,:,2])
    
if __name__ == "__main__":
    
    maxthreads = 16
    starting = os.path.dirname(__file__)
    path = os.path.join(starting, "images")
        
    # Caricamento dele immagini
    images = [np.array(Image.open(os.path.join(path, file)).convert("RGB"), dtype=np.uint64) for file in os.listdir(path)]

        
    start = time.time()
    with ThreadPoolExecutor(max_workers=maxthreads) as executor: #genero i thread e li lancio con pool
        executor.map(integralImage, images ,range(len(images)))
            
    print("--- %s seconds ---" % (time.time() - start))