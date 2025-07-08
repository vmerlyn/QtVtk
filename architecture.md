# QtVTK Project - High-Level UML Class Diagrams

## Overview
This QtVTK project is a 3D model viewer application that integrates Qt Quick with VTK (Visualization Toolkit) for rendering 3D models. The architecture follows a layered approach with clear separation between UI, rendering, and data processing concerns.

## 1. Main Architecture Overview

```mermaid
classDiagram
    class main {
        +main(argc, argv) int
    }
    
    class CanvasHandler {
        -m_processingEngine: shared_ptr~ProcessingEngine~
        -m_vtkFboItem: QVTKFramebufferObjectItem*
        -m_showFileDialog: bool
        -m_draggingMouse: bool
        +CanvasHandler(argc, argv)
        +openModel(path: QUrl)
        +mousePressEvent(button, x, y)
        +mouseMoveEvent(button, x, y)
        +mouseReleaseEvent(button, x, y)
        +setModelsRepresentation(option: int)
        +setModelsOpacity(opacity: double)
        +startApplication()
    }
    
    class QVTKFramebufferObjectItem {
        -m_vtkFboRenderer: QVTKFramebufferObjectRenderer*
        -m_processingEngine: shared_ptr~ProcessingEngine~
        -m_commandsQueue: queue~CommandModel*~
        -m_commandsQueueMutex: mutex
        +createRenderer() Renderer*
        +addModelFromFile(path: QUrl)
        +selectModel(x, y: int)
        +translateModel(params: TranslateParams_t)
        +wheelEvent(e: QWheelEvent*)
        +mousePressEvent(e: QMouseEvent*)
    }
    
    class QVTKFramebufferObjectRenderer {
        -m_processingEngine: shared_ptr~ProcessingEngine~
        -m_vtkRenderWindow: vtkSmartPointer~vtkGenericOpenGLRenderWindow~
        -m_renderer: vtkSmartPointer~vtkRenderer~
        -m_selectedModel: shared_ptr~Model~
        -m_platformModel: vtkSmartPointer~vtkCubeSource~
        +synchronize(item: QQuickFramebufferObject*)
        +render()
        +addModelActor(model: shared_ptr~Model~)
        +selectModel(x, y: int16_t)
        +resetCamera()
        +screenToWorld(screenX, screenY, worldPos[])
    }
    
    main --> CanvasHandler : creates
    CanvasHandler --> QVTKFramebufferObjectItem : manages
    CanvasHandler --> ProcessingEngine : owns
    QVTKFramebufferObjectItem --> QVTKFramebufferObjectRenderer : creates
    QVTKFramebufferObjectItem --> ProcessingEngine : uses
    QVTKFramebufferObjectRenderer --> ProcessingEngine : uses
```

## 2. Data Model and Processing Layer

```mermaid
classDiagram
    class ProcessingEngine {
        -m_models: vector~shared_ptr~Model~~
        +ProcessingEngine()
        +addModel(path: QUrl) shared_ptr~Model~
        +placeModel(model: Model&)
        +setModelsRepresentation(option: int)
        +setModelsOpacity(opacity: double)
        +setModelsGouraudInterpolation(enable: bool)
        +updateModelsColor()
        +getModelFromActor(actor: vtkSmartPointer~vtkActor~) shared_ptr~Model~
        -preprocessPolydata(data: vtkSmartPointer~vtkPolyData~) vtkSmartPointer~vtkPolyData~
    }
    
    class Model {
        -m_modelData: vtkSmartPointer~vtkPolyData~
        -m_modelMapper: vtkSmartPointer~vtkPolyDataMapper~
        -m_modelActor: vtkSmartPointer~vtkActor~
        -m_modelFilterTranslate: vtkSmartPointer~vtkTransformPolyDataFilter~
        -m_positionX, m_positionY, m_positionZ: double
        -m_selected: bool
        -m_propertiesMutex: mutex
        +Model(modelData: vtkSmartPointer~vtkPolyData~)
        +getModelActor() vtkSmartPointer~vtkActor~
        +getPositionX(), getPositionY() double
        +translateToPosition(x, y: double)
        +setSelected(selected: bool)
        +updateModelColor()
        +setMouseDeltaXY(deltaX, deltaY: double)
        -setColor(color: QColor)
    }
    
    ProcessingEngine --> Model : manages collection
    ProcessingEngine --> Model : creates
```

## 3. Command Pattern Implementation

```mermaid
classDiagram
    class CommandModel {
        <<abstract>>
        #m_vtkFboRenderer: QVTKFramebufferObjectRenderer*
        +CommandModel()
        +isReady()* bool
        +execute()* void
    }
    
    class CommandModelAdd {
        -m_processingEngine: shared_ptr~ProcessingEngine~
        -m_model: shared_ptr~Model~
        -m_modelPath: QUrl
        -m_positionX, m_positionY: double
        -m_ready: bool
        +CommandModelAdd(renderer, engine, path)
        +run() override
        +isReady() bool override
        +execute() override
    }
    
    class CommandModelTranslate {
        -m_translateParams: TranslateParams_t
        -m_inTransition: bool
        -m_needsTransformation: bool
        +CommandModelTranslate(renderer, params, inTransition)
        +isReady() bool override
        +execute() override
        -transformCoordinates()
    }
    
    class TranslateParams_t {
        +model: shared_ptr~Model~
        +screenX, screenY: int
        +previousPositionX, previousPositionY: double
        +targetPositionX, targetPositionY: double
    }
    
    CommandModel <|-- CommandModelAdd
    CommandModel <|-- CommandModelTranslate
    CommandModelTranslate --> TranslateParams_t : contains
    CommandModelAdd --|> QThread : inherits
```

## 4. Qt Integration and UI Layer

```mermaid
classDiagram
    class QQuickFramebufferObject {
        <<Qt Framework>>
    }
    
    class QObject {
        <<Qt Framework>>
    }
    
    class QThread {
        <<Qt Framework>>
    }
    
    class QVTKFramebufferObjectItem {
        -m_modelsRepresentationOption: int
        -m_modelsOpacity: double
        -m_gouraudInterpolation: bool
        -m_modelColorR, m_modelColorG, m_modelColorB: int
        +getModelsRepresentation() int
        +getModelsOpacity() double
        +setModelsRepresentation(option: int)
        +setModelsOpacity(opacity: double)
        +setGouraudInterpolation(enable: bool)
        +setModelColorR/G/B(color: int)
    }
    
    class CanvasHandler {
        -m_previousWorldX, m_previousWorldY: double
        +getIsModelSelected() bool
        +getSelectedModelPositionX/Y() double
        +isModelExtensionValid(path: QUrl) bool
    }
    
    QQuickFramebufferObject <|-- QVTKFramebufferObjectItem
    QObject <|-- CanvasHandler
    QObject <|-- Model
    QObject <|-- QVTKFramebufferObjectRenderer
```

## 5. VTK Integration Layer

```mermaid
classDiagram
    class VTKComponents {
        <<VTK Framework>>
        vtkGenericOpenGLRenderWindow
        vtkRenderer
        vtkGenericRenderWindowInteractor
        vtkCellPicker
        vtkActor
        vtkPolyData
        vtkPolyDataMapper
        vtkCubeSource
        vtkTransformPolyDataFilter
    }
    
    class QVTKFramebufferObjectRenderer {
        -m_vtkRenderWindow: vtkSmartPointer~vtkGenericOpenGLRenderWindow~
        -m_renderer: vtkSmartPointer~vtkRenderer~
        -m_vtkRenderWindowInteractor: vtkSmartPointer~vtkGenericRenderWindowInteractor~
        -m_picker: vtkSmartPointer~vtkCellPicker~
        -m_platformModel: vtkSmartPointer~vtkCubeSource~
        -m_platformGrid: vtkSmartPointer~vtkPolyData~
        -m_platformModelActor: vtkSmartPointer~vtkActor~
        -m_platformGridActor: vtkSmartPointer~vtkActor~
        +initScene()
        +generatePlatform()
        +updatePlatform()
        +createLine(x1,y1,z1,x2,y2,z2,points,cells)
    }
    
    class Model {
        -m_modelData: vtkSmartPointer~vtkPolyData~
        -m_modelMapper: vtkSmartPointer~vtkPolyDataMapper~
        -m_modelActor: vtkSmartPointer~vtkActor~
        -m_modelFilterTranslate: vtkSmartPointer~vtkTransformPolyDataFilter~
    }
    
    QVTKFramebufferObjectRenderer --> VTKComponents : uses
    Model --> VTKComponents : uses
    ProcessingEngine --> VTKComponents : uses
```

## 6. Threading and Synchronization

```mermaid
classDiagram
    class ThreadingComponents {
        +Main UI Thread
        +VTK Render Thread
        +Model Loading Thread
    }
    
    class QVTKFramebufferObjectItem {
        -m_commandsQueue: queue~CommandModel*~
        -m_commandsQueueMutex: mutex
        +lockCommandsQueueMutex()
        +unlockCommandsQueueMutex()
        +commandsQueuePop()
        +isCommandsQueueEmpty() bool
    }
    
    class CommandModelAdd {
        +run() override
        +ready() signal
        +done() signal
    }
    
    class Model {
        -m_propertiesMutex: mutex
    }
    
    ThreadingComponents --> QVTKFramebufferObjectItem : synchronizes
    QVTKFramebufferObjectItem --> CommandModelAdd : manages
    CommandModelAdd --|> QThread : runs in separate thread
    Model --> ThreadingComponents : thread-safe access
```

## Key Architecture Patterns

### 1. **MVC Pattern**
- **Model**: `Model` class represents 3D model data
- **View**: Qt Quick QML UI + VTK rendering
- **Controller**: `CanvasHandler` coordinates between UI and data

### 2. **Command Pattern**
- `CommandModel` abstract base class
- `CommandModelAdd` and `CommandModelTranslate` concrete implementations
- Command queue for asynchronous execution

### 3. **Observer Pattern**
- Qt signals/slots for UI updates
- Model position changes notify UI components

### 4. **Facade Pattern**
- `CanvasHandler` provides simplified interface to complex VTK operations
- `ProcessingEngine` encapsulates model processing logic

### 5. **Factory Pattern**
- `QVTKFramebufferObjectItem::createRenderer()` creates renderer instances

## Data Flow Summary

1. **User Interaction** → QML UI → `CanvasHandler`
2. **File Loading** → `CommandModelAdd` → `ProcessingEngine` → `Model`
3. **Model Manipulation** → `CommandModelTranslate` → `Model` → VTK Actors
4. **Rendering** → `QVTKFramebufferObjectRenderer` → VTK Pipeline → OpenGL
5. **UI Updates** → Qt Signals → QML Property Bindings

This architecture provides a clean separation of concerns with proper threading, command pattern for operations, and seamless Qt-VTK integration.