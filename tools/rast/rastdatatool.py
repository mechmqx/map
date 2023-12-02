import cv2
import numpy

height,width,channels = 256,256,3

class tile_id(object):
    def __init__(self,l,x,y) -> None:
        self.level = l
        self.xidx = x
        self.yidx = y


def drawID(image,id):
    font = cv2.FONT_HERSHEY_SIMPLEX
    font_scale = 1.5
    color = (255,255,255)
    thickness = 6    
    position = (80,60)
    cv2.putText(image,str(id.level),position,font,font_scale,color,thickness,cv2.LINE_AA)
    position = (80,140)
    cv2.putText(image,str(id.xidx),position,font,font_scale,color,thickness,cv2.LINE_AA)
    position = (80,220)
    cv2.putText(image,str(id.yidx),position,font,font_scale,color,thickness,cv2.LINE_AA)

def drawLines(image):
    thickness = 4
    pt_tl = (thickness,thickness)
    pt_bl = (thickness,height-thickness)
    pt_br = (width-thickness,height-thickness)
    pt_tr = (width-thickness,thickness)
    lineType = 8    
    color = (128,0,255)
    cv2.line(image, pt_tl, pt_bl, color, thickness, lineType)  # l
    color = (0,128,255)
    cv2.line(image, pt_bl, pt_br, color, thickness, lineType)  # b
    color = (255,128,0)
    cv2.line(image, pt_br, pt_tr, color, thickness, lineType)  # r
    color = (128,255,0)
    cv2.line(image, pt_tr, pt_tl, color, thickness, lineType)  # t


def makeImage(id):    
    image = numpy.zeros((height,width,channels),dtype=numpy.uint8)
    drawID(image,id)
    drawLines(image)
    return image


if __name__ == '__main__':
    '''
    id = tile_id(4,6,8)
    image = makeImage(id)
    cv2.imshow("image",image)
    key = cv2.waitKey(0)
    '''
    max_level = 9
    for l in range(max_level):
        max_x = 2**(l+2)
        max_y = 2**(l+1)
        for x in range(max_x):
            for y in range(max_y):
                id = tile_id(l,x,y)
                image = makeImage(id)
                id_str = './data/'+str(l)+'_'+str(x)+'_'+str(y)+'.png'
                cv2.imwrite(id_str,image)
