# GraphicFramework
A simple library that makes the use of vulkan easier

<br>

<p>Use to load a object from a blender obj file and add a way to transfer data from the memory of host to gpu memory</p>
<br>

```
class TestRender:public LightedRender{

public:

    struct TestBuffer{
      //put data here
    };
    UniformBuffer<WheelUniformBuffer> *buffer;
    TestBuffer hostbuffer;

    
    TestRender(Enviroment& env,
                      string textPath = "textures/texture.png", //here put the right folders
                      string wheelObj = "objs/toLoad.obj"):
           BlenderRender(objPath,textPath,env,false){
    {
        this->vertexPath = "shaders/Vertex.spv";
        this->fragmPath = "shaders/Fragment.spv"; //the names of compiled shaders

        buffer = new UniformBuffer<WheelUniformBuffer>();
        //here you can set the data in the hosted buffer

        buffer->createBuffers(*this->swapChain,hostBuffer);
        addDescriptorSet<WheelUniformBuffer>(*buffer);
        setUp();
    }

    virtual void update(GLFWwindow *window){
      //add the logic you want
    }

};

```
