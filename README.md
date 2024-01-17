# KaChow 3D Engine 123

This project is a 3D Game Engine made by students of CITM for the assigment of Videogame Engine Development of 2023.

It has been made by: Albert Martin and Adria Ramirez

### Our GitHubs:
* [Albert Martin](https://github.com/T4skar)
* [Adrian Ramirez](https://github.com/AdriRamirez)

[Link]([https://github.com/arinWald/KaChowEngine](https://github.com/T4skar/KaChow_3D_Engine)) to our project

## Fortked from other project: 
Project GitHub page: ([https://github.com/arinWald/KaChowEngine)](https://github.com/arinWald/KaChowEngine))


## How to use KaChow 3D Engine?

### Download the engine

1. Download the last release [here]([https://github.com/arinWald/KaChowEngine](https://github.com/arinWald/KaChowEngine/releases/tag/0.5)).
2. Extract files from the .zip
3. Execute .exe

## The engine

### Camera

The camera in KaChow 3D Engine offers various controls for navigating your 3D scene:

- **Right Click:** Enables FPS (First-Person Shooter) free rotation, allowing you to look around in your virtual world with ease.
- **Mouse Wheel UP/DOWN:** Zooms the camera in and out, adjusting the view of your scene.
- **Left Alt + Left Click:** Orbits the camera around the selected object, providing a different perspective.

While right-clicking, the following controls are available to navigate the scene:

- (Holding Shift, movement speed boost)
- **W:** Move forward
- **A:** Move Left
- **S:** Move backward
- **D:** Move Right
- **Q:** Move Up
- **E:** Move Down
- **F:** Focus on the selected object, centering it in the view.
- **Up arrow:** Car accelerate
- **Down arrow:** Car stops
- **Right arrow:** Car go right
- **Left arrow:** Car go left

### Inspector

The 'Inspector' displays all the different components and their properties of the selected GameObjects.

### Components

KaChow Engine supports various components to enhance your 3D objects and assets:

- **Component Transform:** Modify the rotation, position, and scale of the selected GameObject.
- **Component Mesh:** Provides detailed information about different meshes and their vertices for the selected object.
- **Component Materials:** Allows you to view and manage textures and materials applied to your GameObjects.
- **Component Camera:** Edit camera of gameobjects (FOV, Near Distance and Far Distance) and shows number of render objects.
- **Component Physics:** You can make the gameobjects have gravity, you can put a collider on them and make constrains between them.

### Drag & Drop

KaChow Engine simplifies asset management through its Drag & Drop functionality:

- The engine allows users to drag and drop FBX files from anywhere on the hard drive.
- Users can also drag and drop files to assign textures to GameObjects.

### Mouse Picking

Allows the user to select an object by clicking on them in the scene that will be marked in the hierarchy.

### Frustrum Culling

The engine only prints in the scene, the objects that are visibles by the game camera.

### Guizmo

Implementation of Guizmo that allows the user to transform the position, rotation and scale of an object.
IMPORTANT: If the parent's transform of a gameobject is touched, the child will not move properly with gizmo. Use transform on inspector instead.

### Serialization

Implementaion of basic serialization that allowas the user to save and load the scene paramenters.
- Ctrl + S to save the scene
- There's also a button on the header menu

## Windows

### Configuration

In the 'Configuration' section, you can fine-tune various settings to optimize your development and user experience:

**Save and Load Config**
- Two buttons that allow to save and load the current engine configuration.

**FPS Histogram:**
- This option provides a real-time histogram that displays the frames per second (FPS) of your application. Monitoring the FPS is essential for ensuring smooth and responsive gameplay.

**MS Histogram:**
- The MS Histogram shows a histogram of the frame time or DeltaTime. It's a valuable tool for identifying performance bottlenecks and ensuring consistent frame pacing.

**Window:**
- In the 'Window' section, you can manage the display settings:

  - **Window Resolution:** Display the resolution of the window
  - **Display Window Resolution:** View the current window resolution and use two sliders to change it interactively.
  - **Fullscreen Option:** Toggle between windowed and fullscreen display modes

**View:**
- The 'View' section allows you to activate and deactivate various windows within the engine.

**Camera:**
- Change frustum configuration and shows number of render objects.

### Rendering

The rendering capabilities of KaChow Engine allow you to create visually stunning 3D scenes. You can control various rendering options in the 'Render' section:

- **Lighting:** Enable or disable lighting for your scene.
- **Cullface:** Control face culling to optimize rendering.
- **Depth Test:** Toggle depth testing for proper layering.
- **Color Material:** Customize material colors for objects.
- **Wireframe:** Visualize objects in wireframe mode.
- **Textures 2D:** Manage and apply textures to your objects.
- **VSync:** Enable or disable vertical synchronization for smoother rendering.
- **Brightness Slider:** Adjust the brightness of the scene.
- **Bounding Boxes:** Now you can enable or disable the bounding boxes of the game objects.

### Input

- **Mouse and Keyboard Inputs:** Display the current input from the mouse and keyboard, making it easy to understand and manage user interactions.

### Audio

- **Volume Slider:** Adjust the volume to create the desired auditory experience.

### Objects

Create and manipulate objects within your scenes with ease, including primitives and empty GameObjects.

- **Hierarchy:** View and interact with a list of all GameObjects in your scene, making it simple to select and manipulate them.

- **Inspector:** Customize GameObjects by changing their names, viewing mesh information, adjusting materials, and applying textures.

### Console

The 'Console' provides a central location for important logs and messages, making it easy to debug and monitor.

### Viewports

Now you can visualize the Scene Window and the Game Window. Also, full implementation and functionality of Play, Pause and Stop buttons with a cool camera animation.

### Video showing the engine
(https://www.youtube.com/watch?v=esRyybcwZ8I&ab_channel=AlbertMartinLucena)


### External Libraries

Our engine relies on various external libraries to power its functionality:

- [SDL](https://www.libsdl.org/): Simple DirectMedia Layer for cross-platform development.
- [ImGui](https://github.com/ocornut/imgui): The Dear ImGui library for creating user interfaces.
- [Assimp](https://www.assimp.org/): The Open Asset Import Library for 3D model loading.
- [Glew](http://glew.sourceforge.net/): The OpenGL Extension Wrangler Library for OpenGL extensions.
- [MathGeoLib](https://github.com/juj/MathGeoLib): A C++ library for linear algebra and geometry.
- [DevIL](http://openil.sourceforge.net/): The Developer's Image Library for image file handling.
- [PhysFS](https://github.com/kahowell/physfs-cpp): PhysFS++ is a C++ wrapper for the excellent [PhysicsFS library](http://icculus.org/physfs) by Ryan C. Gordon and others.
