#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
using namespace std;

struct Header {
    char idLength;
    char colorMapType;
    char dataTypeCode;
    short colorMapOrigin;
    short colorMapLength;
    char colorMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char bitsPerPixel;
    char imageDescriptor;
};

struct Pixel{
    unsigned char BLUE;
    unsigned char GREEN;
    unsigned char RED;
};

struct Image {
    Header header;
    vector<Pixel> pixelData;
};

Image ReadTGA(const string& path) {
    ifstream file(path, ios::binary);
    Image image;
    if (!file.is_open()) {
        cerr << "Error: Input file " << path << " not found." << endl;
        return image;
    }
    Header header;
    file.read(&header.idLength, sizeof(header.idLength));
    file.read(&header.colorMapType, sizeof(header.colorMapType));
    file.read(&header.dataTypeCode, sizeof(header.dataTypeCode));
    file.read(reinterpret_cast<char*>(&header.colorMapOrigin), sizeof(header.colorMapOrigin));
    file.read(reinterpret_cast<char*>(&header.colorMapLength), sizeof(header.colorMapLength));
    file.read(&header.colorMapDepth, sizeof(header.colorMapDepth));
    file.read(reinterpret_cast<char*>(&header.xOrigin), sizeof(header.xOrigin));
    file.read(reinterpret_cast<char*>(&header.yOrigin), sizeof(header.yOrigin));
    file.read(reinterpret_cast<char*>(&header.width), sizeof(header.width));
    file.read(reinterpret_cast<char*>(&header.height), sizeof(header.height));
    file.read(&header.bitsPerPixel, sizeof(header.bitsPerPixel));
    file.read(&header.imageDescriptor, sizeof(header.imageDescriptor));
    image.header = header;
    if (header.width <= 0 || header.height <= 0) {
        cerr << "Error: Invalid image dimensions" << endl;
        return image;
    }
    int pixCount = header.width * header.height;
    for (int i = 0; i < pixCount; i++) {
        Pixel pixel;
        file.read(reinterpret_cast<char*>(&pixel.BLUE), sizeof(pixel.BLUE));
        file.read(reinterpret_cast<char*>(&pixel.GREEN), sizeof(pixel.GREEN));
        file.read(reinterpret_cast<char*>(&pixel.RED), sizeof(pixel.RED));
        image.pixelData.push_back(pixel);
    }
    file.close();
    return image;
}

void WriteFile(Image& image,const string& outputPath){
    ofstream outputFile(outputPath, ios::binary);
    if(!outputFile.is_open()) {
        cout << "Failed to open output file: " << outputPath << endl;
        return;
    }
    outputFile.write(&image.header.idLength, sizeof(image.header.idLength));
    outputFile.write(&image.header.colorMapType, sizeof(image.header.colorMapType));
    outputFile.write(&image.header.dataTypeCode, sizeof(image.header.dataTypeCode));
    outputFile.write(reinterpret_cast<char*>(&image.header.colorMapOrigin), sizeof(image.header.colorMapOrigin));
    outputFile.write(reinterpret_cast<char*>(&image.header.colorMapLength), sizeof(image.header.colorMapLength));
    outputFile.write(&image.header.colorMapDepth, sizeof(image.header.colorMapDepth));
    outputFile.write(reinterpret_cast<char*>(&image.header.xOrigin), sizeof(image.header.xOrigin));
    outputFile.write(reinterpret_cast<char*>(&image.header.yOrigin), sizeof(image.header.yOrigin));
    outputFile.write(reinterpret_cast<char*>(&image.header.width), sizeof(image.header.width));
    outputFile.write(reinterpret_cast<char*>(&image.header.height), sizeof(image.header.height));
    outputFile.write(&image.header.bitsPerPixel, sizeof(image.header.bitsPerPixel));
    outputFile.write(&image.header.imageDescriptor, sizeof(image.header.imageDescriptor));
    for(int i = 0; i < (image.header.width * image.header.height); i++) {
        Pixel currPix = image.pixelData[i];
        unsigned char currBlue = currPix.BLUE;
        unsigned char currGreen = currPix.GREEN;
        unsigned char currRed = currPix.RED;
        outputFile.write(reinterpret_cast<char*>(&currBlue), sizeof(currBlue));
        outputFile.write(reinterpret_cast<char*>(&currGreen), sizeof(currGreen));
        outputFile.write(reinterpret_cast<char*>(&currRed), sizeof(currRed));
    }
    outputFile.close();
}

void notInt() {
    cerr << "Invalid argument, expected a number." << endl;
}

Image scaleblue(Image& im, string scale){
    int mult = 0;
    try {
        mult = stoi(scale);
    }catch (const invalid_argument& e) {
        notInt();
    }
    Image final = im;
    for(int i = 0; i < (im.header.width * im.header.height); i++){
        int b = static_cast<int>(final.pixelData[i].BLUE) * mult;
        if(b > 255){
            b = 255;
        }
        final.pixelData[i].BLUE = static_cast<unsigned char>(b);
    }
    return final;
}

Image scalegreen(Image& im, string scale){
    int mult = 0;
    try {
        mult = stoi(scale);
    }catch (const invalid_argument& e) {
        notInt();
    }
    Image final = im;
    for(int i = 0; i < (im.header.width * im.header.height); i++){
        int g = static_cast<int>(final.pixelData[i].GREEN) * mult;
        if(g > 255) {
            g = 255;
        }
        final.pixelData[i].GREEN = static_cast<unsigned char>(g);
    }
    return final;
}

Image scalered(Image& im, string scale){
    int mult = 0;
    try {
        mult = stoi(scale);
    }catch (const invalid_argument& e) {
        notInt();
    }
    Image final = im;
    for(int i = 0; i < (im.header.width * im.header.height); i++){
        int r = static_cast<int>(final.pixelData[i].RED) * mult;
        if(r > 255){
            r = 255;
        }
        final.pixelData[i].RED = static_cast<unsigned char>(r);
    }
    return final;
}

Image multiply(Image& im, Image& im2){
    if(im.header.width != im2.header.width || im.header.height != im2.header.height){
        cout << "Error, images are not the same size." << endl;
        return Image();
    }
    Image final = im;
    for (int i = 0; i < im.header.width * im.header.height; i++){
        Pixel& pixel = im.pixelData[i];
        Pixel& pixel2 = im2.pixelData[i];
        Pixel newpix;
        float b = (((static_cast<float>(pixel.BLUE) / 255) * (static_cast<float>(pixel2.BLUE) / 255)) * 255) + 0.5;
        float g = (((static_cast<float>(pixel.GREEN) / 255) * (static_cast<float>(pixel2.GREEN) / 255)) * 255) + 0.5;
        float r = (((static_cast<float>(pixel.RED) / 255) * (static_cast<float>(pixel2.RED) / 255)) * 255) + 0.5;
        newpix.BLUE = static_cast<unsigned char>(b);
        newpix.GREEN = static_cast<unsigned char>(g);
        newpix.RED = static_cast<unsigned char>(r);
        final.pixelData[i] = newpix;
    }
    return final;
}

Image screen(Image& im, Image& im2){
    if(im.header.width != im2.header.width || im.header.height != im2.header.height){
        cout << "Error, images are not the same size." << endl;
        return Image();
    }
    Image final = im;
    for (int i = 0; i < im.header.width * im.header.height; i++){
        Pixel& pixel = im.pixelData[i];
        Pixel& pixel2 = im2.pixelData[i];
        Pixel newpix;
        float b = (1 - (((1 - (static_cast<float>(pixel.BLUE) / 255)) * (1 - (static_cast<float>(pixel2.BLUE) / 255))) /*+ 0.5*/)) * 255 + 0.5;
        float g = (1 - (((1 - (static_cast<float>(pixel.GREEN) / 255)) * (1 - (static_cast<float>(pixel2.GREEN) / 255))) /*+ 0.5*/)) * 255 + 0.5;
        float r = (1 - (((1 - (static_cast<float>(pixel.RED) / 255)) * (1 - (static_cast<float>(pixel2.RED) / 255))) /*+ 0.5*/)) * 255 + 0.5;
        newpix.BLUE = static_cast<unsigned char>(b);
        newpix.GREEN = static_cast<unsigned char>(g);
        newpix.RED = static_cast<unsigned char>(r);
        final.pixelData[i] = newpix;
    }
    return final;
}

Image subtract(Image& im, Image& im2) {
    if(im.header.width != im2.header.width || im.header.height != im2.header.height){
        cout << "Error, images are not the same size." << endl;
        return Image();
    }
    Image final = im;
    for (int i = 0; i < im.header.width * im.header.height; i++){
        Pixel& pixel = im.pixelData[i];
        Pixel& pixel2 = im2.pixelData[i];
        Pixel newpix;
        int b = static_cast<int>(pixel.BLUE) - (static_cast<int>(pixel2.BLUE));
        int g = static_cast<int>(pixel.GREEN) - (static_cast<int>(pixel2.GREEN));
        int r = static_cast<int>(pixel.RED) - (static_cast<int>(pixel2.RED));
        if(b < 0){
            b = 0;
        }
        if(g < 0){
            g = 0;
        }
        if(r < 0){
            r = 0;
        }
        newpix.BLUE = static_cast<unsigned char>(b);
        newpix.GREEN = static_cast<unsigned char>(g);
        newpix.RED = static_cast<unsigned char>(r);
        final.pixelData[i] = newpix;
    }
    return final;
}

Image addblue(Image& im, string scale){
    int add = 0;
    try {
        add = stoi(scale);
    }catch (const invalid_argument& e) {
        notInt();
    }
    Image final = im;
    for(int i = 0; i < (im.header.width * im.header.height); i++){
        int b = static_cast<int>(final.pixelData[i].BLUE) + add;
        if(b > 255) {
            b = 255;
        }
        if (b < 0) {
            b = 0;
        }
        final.pixelData[i].BLUE = static_cast<unsigned char>(b);
    }
    return final;
}

Image addgreen(Image& im, string scale){
    int add = 0;
    try {
        add = stoi(scale);
    }catch (const invalid_argument& e) {
        notInt();
    }
    Image final = im;
    for(int i = 0; i < (im.header.width * im.header.height); i++){
        int g = static_cast<int>(final.pixelData[i].GREEN) + add;
        if(g > 255) {
            g = 255;
        }
        if (g < 0) {
            g = 0;
        }
        final.pixelData[i].GREEN = static_cast<unsigned char>(g);
    }
    return final;
}

Image addred(Image& im, string scale){
    int add = 0;
    try {
        add = stoi(scale);
    }catch (const invalid_argument& e) {
        notInt();
    }
    Image final = im;
    for(int i = 0; i < (im.header.width * im.header.height); i++){
        int r = static_cast<int>(final.pixelData[i].RED) + add;
        if(r > 255) {
            r = 255;
        }
        if (r < 0) {
            r = 0;
        }
        final.pixelData[i].RED = static_cast<unsigned char>(r);
    }
    return final;
}

Image Addition(Image& im, Image& im2) {
    if(im.header.width != im2.header.width || im.header.height != im2.header.height){
        cout << "Error, images are not the same size." << endl;
        return Image();
    }
    Image final = im;
    for (int i = 0; i < im.header.width * im.header.height; i++){
        Pixel& pixel = im.pixelData[i];
        Pixel& pixel2 = im2.pixelData[i];
        Pixel newpix;
        int b = static_cast<int>(pixel.BLUE) + (static_cast<int>(pixel2.BLUE));
        int g = static_cast<int>(pixel.GREEN) + (static_cast<int>(pixel2.GREEN));
        int r = static_cast<int>(pixel.RED) + (static_cast<int>(pixel2.RED));
        if(b > 255){
            b = 255;
        }
        if(g > 255){
            g = 255;
        }
        if(r > 255){
            r = 255;
        }
        newpix.BLUE = static_cast<unsigned char>(b);
        newpix.GREEN = static_cast<unsigned char>(g);
        newpix.RED = static_cast<unsigned char>(r);
        final.pixelData[i] = newpix;
    }
    return final;
}

Image overlay(Image& im, Image& im2) {
    if(im.header.width != im2.header.width || im.header.height != im2.header.height){
        cout << "Error, images are not the same size." << endl;
        return Image();
    }
    Image final = im;
    for (int i = 0; i < im.header.width * im.header.height; i++){
        Pixel& pixel = im.pixelData[i];
        Pixel& pixel2 = im2.pixelData[i];
        Pixel newpix;
        float b;
        float g;
        float r;
        if((static_cast<float>(pixel2.BLUE) / 255) <= 0.5) {
            b = ((static_cast<float>(pixel.BLUE) / 255) * (static_cast<float>(pixel2.BLUE) / 255) * 2) * 255;
        }else{
            b = (1 - (2 * (1 - (static_cast<float>(pixel.BLUE) / 255)) * (1 - ((static_cast<float>(pixel2.BLUE) / 255))))) * 255;
        }
        if((static_cast<float>(pixel2.GREEN) / 255) <= 0.5) {
            g = ((static_cast<float>(pixel.GREEN) / 255) * (static_cast<float>(pixel2.GREEN) / 255) * 2) * 255;
        }else{
            g = (1 - (2 * (1 - (static_cast<float>(pixel.GREEN) / 255)) * (1 - ((static_cast<float>(pixel2.GREEN) / 255))))) * 255;
        }
        if((static_cast<float>(pixel2.RED) / 255) <= 0.5) {
            r = ((static_cast<float>(pixel.RED) / 255) * (static_cast<float>(pixel2.RED) / 255) * 2) * 255;
        }else{
            r = (1 - (2 * (1 - (static_cast<float>(pixel.RED) / 255)) * (1 - ((static_cast<float>(pixel2.RED) / 255))))) * 255;
        }
        newpix.BLUE = static_cast<unsigned char>(b);
        newpix.GREEN = static_cast<unsigned char>(g);
        newpix.RED = static_cast<unsigned char>(r);
        final.pixelData[i] = newpix;
    }
    return final;
}

Image SepCols(Image& im, char color) {
    Image final = im;
    for(int i = 0; i < (im.header.width * im.header.height); i++) {
        Pixel& pixel = im.pixelData[i];
        Pixel newpix;
        if(color == 'B') {
            newpix.RED = pixel.BLUE;
            newpix.GREEN = pixel.BLUE;
            newpix.BLUE = pixel.BLUE;
        }
        if(color == 'G') {
            newpix.RED = pixel.GREEN;
            newpix.GREEN = pixel.GREEN;
            newpix.BLUE = pixel.GREEN;
        }
        if(color == 'R') {
            newpix.RED = pixel.RED;
            newpix.GREEN = pixel.RED;
            newpix.BLUE = pixel.RED;
        }
        final.pixelData[i] = newpix;
    }
    return final;
}

Image onlyred(Image& im) {
    Image final = im;
    for (int i = 0; i < im.header.width * im.header.height; i++) {
        Pixel& pixel = im.pixelData[i];
        Pixel newpix;
        newpix.RED = pixel.RED;
        newpix.GREEN = pixel.RED;
        newpix.BLUE = pixel.RED;
        final.pixelData[i] = newpix;
    }
    return final;
}

Image onlygreen(Image& im) {
    Image final = im;
    for (int i = 0; i < im.header.width * im.header.height; i++) {
        Pixel& pixel = im.pixelData[i];
        Pixel newpix;
        newpix.RED = pixel.GREEN;
        newpix.GREEN = pixel.GREEN;
        newpix.BLUE = pixel.GREEN;
        final.pixelData[i] = newpix;
    }
    return final;
}

Image onlyblue(Image& im) {
    Image final = im;
    for (int i = 0; i < im.header.width * im.header.height; i++) {
        Pixel& pixel = im.pixelData[i];
        Pixel newpix;
        newpix.RED = pixel.BLUE;
        newpix.GREEN = pixel.BLUE;
        newpix.BLUE = pixel.BLUE;
        final.pixelData[i] = newpix;
    }
    return final;
}

Image combine(Image& im1, Image& im2, Image& im3){
    if((im1.header.width != im2.header.width) || (im1.header.height != im2.header.height) || (im1.header.height != im3.header.height) || (im1.header.width != im3.header.width) || (im2.header.height != im3.header.height) || (im2.header.width != im3.header.width)){
        cout << "Error, images are not the same size." << endl;
        return Image();
    }
    //cout << im1.header.width << " " << im1.header.height << endl;
    Image final = im1;
    for(int i = 0; i < (im1.header.width * im1.header.height); i++) {
        Pixel& pixel = im1.pixelData[i];
        Pixel& pixel2 = im2.pixelData[i];
        Pixel& pixel3 = im3.pixelData[i];
        Pixel newpix;
        newpix.BLUE = pixel3.BLUE;
        newpix.GREEN = pixel2.GREEN;
        newpix.RED = pixel.RED;
        final.pixelData[i] = newpix;
    }
    return final;
}

Image flip(Image& im) {
    Image final = im;
    //Pixel newpix;
    int x = im.header.width;
    int y = im.header.height;
    for(int i = 0; i < (x*y); i++){
        int o = y*x - 1 - i;
        int bot = (y - 1 - i) * x;
        //for (int j = 0; j < x; j++) {
          //  int topp = top + j;
            //int bott = bot + (x - 1 - j);
            Pixel temp = final.pixelData[i];
            final.pixelData[i] = final.pixelData[o];
            final.pixelData[o] = temp;
            //temp = final.pixelData[topR];
            //final.pixelData[topR] = final.pixelData[botL];
            //final.pixelData[botL] = temp;
        //}
    }
    return final;
}

/*
int main() {
    string path1 = "cmake-build-debug/input/car.tga";
    Image image1 = ReadTGA(path1);
    string path2 = "cmake-build-debug/input/circles.tga";
    Image image2 = ReadTGA(path2);
    string path3 = "cmake-build-debug/input/layer1.tga";
    Image image3 = ReadTGA(path3);
    string path4 = "cmake-build-debug/input/layer2.tga";
    Image image4 = ReadTGA(path4);
    string path5 = "cmake-build-debug/input/layer_blue.tga";
    Image image5 = ReadTGA(path5);
    string path6 = "cmake-build-debug/input/layer_green.tga";
    Image image6 = ReadTGA(path6);
    string path7 = "cmake-build-debug/input/layer_red.tga";
    Image image7 = ReadTGA(path7);
    string path8 = "cmake-build-debug/input/pattern1.tga";
    Image image8 = ReadTGA(path8);
    string path9 = "cmake-build-debug/input/pattern2.tga";
    Image image9 = ReadTGA(path9);
    string path10 = "cmake-build-debug/input/text.tga";
    Image image10 = ReadTGA(path10);
    string path11 = "cmake-build-debug/input/text2.tga";\
    Image image11 = ReadTGA(path11);

    string outpath1 = "cmake-build-debug/output/part1.tga";
    Image newimg1 = Multiply(image3, image8);
    WriteFile(newimg1, outpath1);

    string outpath2 = "cmake-build-debug/output/part2.tga";
    Image newimg2 = Subtract(image1, image4);
    WriteFile(newimg2, outpath2);

    string outpath3 = "cmake-build-debug/output/part3.tga";
    Image tempimg1 = Multiply(image3, image9);
    Image newimg3 = Screen(image10, tempimg1);
    WriteFile(newimg3, outpath3);

    string outpath4 = "cmake-build-debug/output/part4.tga";
    Image tempimg2 = Multiply(image4, image2);
    Image newimg4 = Subtract(tempimg2, image9);
    WriteFile(newimg4, outpath4);

    string outpath5 = "cmake-build-debug/output/part5.tga";
    Image newimg5 = Overlay(image3, image8);
    WriteFile(newimg5, outpath5);

    string outpath6 = "cmake-build-debug/output/part6.tga";
    Image newimg6 = AddGreen(image1, 200);
    WriteFile(newimg6, outpath6);

    string outpath7 = "cmake-build-debug/output/part7.tga";
    Image temp = MultRed(image1, 4);
    Image newimg7 = MultBlue(temp, 0);
    WriteFile(newimg7, outpath7);

    string outpath8b = "cmake-build-debug/output/part8_b.tga";
    Image newimg8b = SepCols(image1, 'B');
    WriteFile(newimg8b, outpath8b);

    string outpath8g = "cmake-build-debug/output/part8_g.tga";
    Image newimg8g = SepCols(image1, 'G');
    WriteFile(newimg8g, outpath8g);

    string outpath8r = "cmake-build-debug/output/part8_r.tga";
    Image newimg8r = SepCols(image1, 'R');
    WriteFile(newimg8r, outpath8r);

    string outpath9 = "cmake-build-debug/output/part9.tga";
    Image newimg9 = Combine(image7, image6, image5);
    WriteFile(newimg9, outpath9);

    string outpath10 = "cmake-build-debug/output/part10.tga";
    Image newimg10 = UpsideDown(image11);
    WriteFile(newimg10, outpath10);     

    return 0;
}*/
void help() {
    cout << "Project 2: Image Processing, Spring 2025" << endl;
    cout << "Usage:" << endl;
    cout << "\t ./project2.out [output] [firstImage] [method] [...]" << endl;
}
void noMName() {
    cerr << "Invalid method name." << endl;
}
void noArgs() {
    cerr << "Missing argument." << endl;
}
void invfile() {
    cerr << "Invalid argument, invalid file name." << endl;
}
void noFile() {
    cerr << "Invalid argument, file does not exist." << endl;
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
        help();
        return 0;
    }
    vector<string> args;
    for(int i = 0; i < argc; i++) {
        args.push_back((string)(argv[i]));
    }
    int methodIndex = 3;
    if (args[2].find(".tga") != args[2].size() - 4) {
        invfile();
        return 1;
    }
    Image tracking = ReadTGA(args[2]);
    Image final = tracking;
    if (tracking.pixelData.empty()) {
        noFile();
        return 1;
    }
    while (methodIndex < argc) {
        if (args[methodIndex] == "onlyred") {
            final = onlyred(final);
            methodIndex++;
        }else if (args[methodIndex] == "onlygreen") {
            final = onlygreen(final);
            methodIndex++;
        }else if (args[methodIndex] == "onlyblue") {
            final = onlyblue(final);
            methodIndex++;
        }else if (args[methodIndex] == "flip") {
            final = flip(final);
            methodIndex++;
        }else if (args[methodIndex] == "multiply") {
            if (methodIndex + 1 >= argc) {
                noArgs();
                return 1;
            }
            if (args[methodIndex+1].find(".tga") != args[methodIndex+1].size() - 4) {
                 invfile();
                 return 1;
             }
            Image img2 = ReadTGA(args[methodIndex + 1]);
            if (img2.pixelData.empty()) {
                 noFile();
                 return 1;
             }
            final = multiply(final, img2);
            methodIndex += 2;
        }else if (args[methodIndex] == "subtract") {
            if (methodIndex + 1 >= argc) {
                noArgs();
                return 1;
            }
            if (args[methodIndex+1].find(".tga") != args[methodIndex+1].size() - 4) {
                invfile();
                return 1;
            }
            Image img2 = ReadTGA(args[methodIndex + 1]);
            if (img2.pixelData.empty()) {
                noFile();
                return 1;
            }
            final = subtract(final, img2);
            methodIndex += 2;
        }else if (args[methodIndex] == "overlay") {
            if (methodIndex + 1 >= argc) {
                noArgs();
                return 1;
            }
            if (args[methodIndex+1].find(".tga") != args[methodIndex+1].size() - 4) {
                invfile();
                return 1;
            }
            Image img2 = ReadTGA(args[methodIndex + 1]);
            if (img2.pixelData.empty()) {
                noFile();
                return 1;
            }
            final = overlay(final, img2);
            methodIndex += 2;
        }else if (args[methodIndex] == "screen") {
            if (methodIndex + 1 >= argc) {
                noArgs();
                return 1;
            }
            if (args[methodIndex+1].find(".tga") != args[methodIndex+1].size() - 4) {
                invfile();
                return 1;
            }
            Image img2 = ReadTGA(args[methodIndex + 1]);
            if (img2.pixelData.empty()) {
                noFile();
                return 1;
            }
            final = screen(final, img2);
            methodIndex += 2;
        }else if (args[methodIndex] == "addred") {
            if (methodIndex + 1 >= argc) {
                noArgs();
                return 1;
            }
            string amt = args[methodIndex + 1];
            final = addred(final, amt);
            methodIndex += 2;
        }else if (args[methodIndex] == "addgreen") {
            if (methodIndex + 1 >= argc) {
                noArgs();
                return 1;
            }
            string amt = args[methodIndex + 1];
            final = addgreen(final, amt);
            methodIndex += 2;
        }else if (args[methodIndex] == "addblue") {
            if (methodIndex + 1 >= argc) {
                noArgs();
                return 1;
            }
            string amt = args[methodIndex + 1];
            final = addblue(final, amt);
            methodIndex += 2;
        }else if (args[methodIndex] == "scalered") {
            if (methodIndex + 1 >= argc) {
                noArgs();
                return 1;
            }
            string amt = args[methodIndex + 1];
            final = scalered(final, amt);
            methodIndex += 2;
        }else if (args[methodIndex] == "scalegreen") {
            if (methodIndex + 1 >= argc) {
                noArgs();
                return 1;
            }
            string amt = args[methodIndex + 1];
            final = scalegreen(final, amt);
            methodIndex += 2;
        }else if (args[methodIndex] == "scaleblue") {
            if (methodIndex + 1 >= argc) {
                noArgs();
                return 1;
            }
            string amt = args[methodIndex + 1];
            final = scaleblue(final, amt);
            methodIndex += 2;
        }else if (args[methodIndex] == "combine") {
            if (methodIndex + 2 >= argc) {
                noArgs();
                return 1;
            }
            if ((args[methodIndex + 1].find(".tga") != args[methodIndex + 1].size() - 4) || (args[methodIndex + 2].find(".tga") != args[methodIndex + 2].size() - 4)) {
                invfile();
                return 1;
            }
            Image img2 = ReadTGA(args[methodIndex + 1]);
            Image img3 = ReadTGA(args[methodIndex + 2]);
            if (img2.pixelData.empty() || img3.pixelData.empty()) {
                noFile();
                return 1;
            }
            final = combine(tracking, img2, img3);
            methodIndex += 3;
        }else {
            noMName();
            return 1;
        }

    }
    WriteFile(final, args[1]);
    return 0;
}

