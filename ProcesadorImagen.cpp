#include "ProcesadorImagen.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <memory>

void ProcesadorImagen::invertirColores(Archivo_jpeg& archivo) {
    auto inicio = std::chrono::high_resolution_clock::now();
    
    std::cout << "  > Invirtiendo colores de la imagen: " << archivo.obtenerNombreArchivo() << std::endl;
    const auto& datosOriginales = archivo.obtenerDatosImagen();
    std::vector<unsigned char> datosModificados = datosOriginales;
    int ancho = archivo.obtenerAncho();
    int alto = archivo.obtenerAlto();
    int numComponentes = archivo.obtenerNumComponentes();

    for (int y = 0; y < alto; ++y) {
        for (int x = 0; x < ancho; ++x) {
            for (int c = 0; c < numComponentes; ++c) {
                int indice = (y * ancho + x) * numComponentes + c;
                datosModificados[indice] = 255 - datosOriginales[indice];
            }
        }
    }
    
    Archivo_jpeg archivoModificado = archivo;
    archivoModificado.actualizarDatosImagen(datosModificados);
    archivoModificado.guardarImagen("_invertido");
    
    auto fin = std::chrono::high_resolution_clock::now();
    double tiempo = std::chrono::duration<double, std::milli>(fin - inicio).count();
    guardarTiempoProcesamiento("invertirColores", tiempo);
    std::cout << "  ...Tiempo de procesamiento (invertirColores): " << tiempo << " ms" << std::endl;
}

void ProcesadorImagen::binarizarImagen(Archivo_jpeg& archivo) {
    auto inicio = std::chrono::high_resolution_clock::now();
    
    std::cout << "  > Binarizando la imagen: " << archivo.obtenerNombreArchivo() << std::endl;
    const auto& datosOriginales = archivo.obtenerDatosImagen();
    std::vector<unsigned char> datosModificados = datosOriginales;
    int ancho = archivo.obtenerAncho();
    int alto = archivo.obtenerAlto();
    int numComponentes = archivo.obtenerNumComponentes();
    
    for (int y = 0; y < alto; ++y) {
        for (int x = 0; x < ancho; ++x) {
            int promedio = 0;
            for (int c = 0; c < numComponentes; ++c) {
                promedio += datosOriginales[(y * ancho + x) * numComponentes + c];
            }
            promedio /= numComponentes;
            
            unsigned char valor = (promedio > 128) ? 255 : 0;
            for (int c = 0; c < numComponentes; ++c) {
                datosModificados[(y * ancho + x) * numComponentes + c] = valor;
            }
        }
    }
    
    Archivo_jpeg archivoModificado = archivo;
    archivoModificado.actualizarDatosImagen(datosModificados);
    archivoModificado.guardarImagen("_binarizado");
    
    auto fin = std::chrono::high_resolution_clock::now();
    double tiempo = std::chrono::duration<double, std::milli>(fin - inicio).count();
    guardarTiempoProcesamiento("binarizarImagen", tiempo);
    std::cout << "  ...Tiempo de procesamiento (binarizarImagen): " << tiempo << " ms" << std::endl;
}

void ProcesadorImagen::espejoImagen(Archivo_jpeg& archivo) {
    auto inicio = std::chrono::high_resolution_clock::now();
    
    std::cout << "  > Aplicando efecto espejo a la imagen: " << archivo.obtenerNombreArchivo() << std::endl;
    const auto& datosOriginales = archivo.obtenerDatosImagen();
    std::vector<unsigned char> datosModificados = datosOriginales;
    int ancho = archivo.obtenerAncho();
    int alto = archivo.obtenerAlto();
    int numComponentes = archivo.obtenerNumComponentes();
    
    for (int y = 0; y < alto; ++y) {
        for (int x = 0; x < ancho / 2; ++x) {
            for (int c = 0; c < numComponentes; ++c) {
                std::swap(datosModificados[(y * ancho + x) * numComponentes + c],
                          datosModificados[(y * ancho + (ancho - 1 - x)) * numComponentes + c]);
            }
        }
    }
    
    Archivo_jpeg archivoModificado = archivo;
    archivoModificado.actualizarDatosImagen(datosModificados);
    archivoModificado.guardarImagen("_espejo");
    
    auto fin = std::chrono::high_resolution_clock::now();
    double tiempo = std::chrono::duration<double, std::milli>(fin - inicio).count();
    guardarTiempoProcesamiento("espejoImagen", tiempo);
    std::cout << "  ...Tiempo de procesamiento (espejoImagen): " << tiempo << " ms" << std::endl;
}

void ProcesadorImagen::aplicarKernelConvolucion(Archivo_jpeg& archivo) {
    auto inicio = std::chrono::high_resolution_clock::now();
    
    std::cout << "  > Aplicando kernel de convolución a la imagen: " << archivo.obtenerNombreArchivo() << std::endl;
    const auto& datosOriginales = archivo.obtenerDatosImagen();
    int ancho = archivo.obtenerAncho();
    int alto = archivo.obtenerAlto();
    int numComponentes = archivo.obtenerNumComponentes();
    
    // Kernel de ejemplo (detección de bordes)
    const int kernel[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };
    
    std::vector<unsigned char> datosModificados(datosOriginales.size());
    
    for (int y = 1; y < alto - 1; ++y) {
        for (int x = 1; x < ancho - 1; ++x) {
            for (int c = 0; c < numComponentes; ++c) {
                int suma = 0;
                for (int ky = -1; ky <= 1; ++ky) {
                    for (int kx = -1; kx <= 1; ++kx) {
                        int indice = ((y + ky) * ancho + (x + kx)) * numComponentes + c;
                        suma += datosOriginales[indice] * kernel[ky + 1][kx + 1];
                    }
                }
                suma = std::max(0, std::min(suma, 255));
                datosModificados[(y * ancho + x) * numComponentes + c] = static_cast<unsigned char>(suma);
            }
        }
    }
    
    Archivo_jpeg archivoModificado = archivo;
    archivoModificado.actualizarDatosImagen(datosModificados);
    archivoModificado.guardarImagen("_convolucion");
    
    auto fin = std::chrono::high_resolution_clock::now();
    double tiempo = std::chrono::duration<double, std::milli>(fin - inicio).count();
    guardarTiempoProcesamiento("aplicarKernelConvolucion", tiempo);
    std::cout << "  ...Tiempo de procesamiento (aplicarKernelConvolucion): " << tiempo << " ms" << std::endl;
}

void ProcesadorImagen::guardarTiempoProcesamiento(const std::string& nombreFuncion, double tiempo) {
    std::ofstream archivo("tiempos_procesamiento.txt", std::ios::app);
    if (archivo.is_open()) {
        archivo << nombreFuncion << ": " << tiempo << " ms" << std::endl;
        archivo.close();
    } else {
        std::cerr << "No se pudo abrir el archivo para guardar los tiempos de procesamiento." << std::endl;
    }
}

void ProcesadorImagen::invertirColoresOpenMP(Archivo_jpeg& archivo) {
    auto inicio = std::chrono::high_resolution_clock::now();
    
    std::cout << "  > Invirtiendo colores de la imagen (OpenMP): " << archivo.obtenerNombreArchivo() << std::endl;
    const auto& datosOriginales = archivo.obtenerDatosImagen();
    std::vector<unsigned char> datosModificados = datosOriginales;
    int tamanio = datosOriginales.size();

    #pragma omp parallel for
    for (int i = 0; i < tamanio; ++i) {
        datosModificados[i] = 255 - datosOriginales[i];
    }
    
    Archivo_jpeg archivoModificado = archivo;
    archivoModificado.actualizarDatosImagen(datosModificados);
    archivoModificado.guardarImagen("_invertido_omp");
    
    auto fin = std::chrono::high_resolution_clock::now();
    double tiempo = std::chrono::duration<double, std::milli>(fin - inicio).count();
    guardarTiempoProcesamiento("invertirColoresOpenMP", tiempo);
    std::cout << "  ...Tiempo de procesamiento (invertirColoresOpenMP): " << tiempo << " ms" << std::endl;
}

void ProcesadorImagen::binarizarImagenOpenMP(Archivo_jpeg& archivo) {
    auto inicio = std::chrono::high_resolution_clock::now();
    
    std::cout << "  > Binarizando la imagen (OpenMP): " << archivo.obtenerNombreArchivo() << std::endl;
    const auto& datosOriginales = archivo.obtenerDatosImagen();
    std::vector<unsigned char> datosModificados = datosOriginales;
    int ancho = archivo.obtenerAncho();
    int alto = archivo.obtenerAlto();
    int numComponentes = archivo.obtenerNumComponentes();
    
    #pragma omp parallel for collapse(2)
    for (int y = 0; y < alto; ++y) {
        for (int x = 0; x < ancho; ++x) {
            int promedio = 0;
            for (int c = 0; c < numComponentes; ++c) {
                promedio += datosOriginales[(y * ancho + x) * numComponentes + c];
            }
            promedio /= numComponentes;
            
            unsigned char valor = (promedio > 128) ? 255 : 0;
            for (int c = 0; c < numComponentes; ++c) {
                datosModificados[(y * ancho + x) * numComponentes + c] = valor;
            }
        }
    }
    
    Archivo_jpeg archivoModificado = archivo;
    archivoModificado.actualizarDatosImagen(datosModificados);
    archivoModificado.guardarImagen("_binarizado_omp");
    
    auto fin = std::chrono::high_resolution_clock::now();
    double tiempo = std::chrono::duration<double, std::milli>(fin - inicio).count();
    guardarTiempoProcesamiento("binarizarImagenOpenMP", tiempo);
    std::cout << "  ...Tiempo de procesamiento (binarizarImagenOpenMP): " << tiempo << " ms" << std::endl;
}

void ProcesadorImagen::espejoImagenOpenMP(Archivo_jpeg& archivo) {
    auto inicio = std::chrono::high_resolution_clock::now();
    
    std::cout << "  > Aplicando efecto espejo a la imagen (OpenMP): " << archivo.obtenerNombreArchivo() << std::endl;
    const auto& datosOriginales = archivo.obtenerDatosImagen();
    std::vector<unsigned char> datosModificados = datosOriginales;
    int ancho = archivo.obtenerAncho();
    int alto = archivo.obtenerAlto();
    int numComponentes = archivo.obtenerNumComponentes();
    
    #pragma omp parallel for
    for (int y = 0; y < alto; ++y) {
        for (int x = 0; x < ancho / 2; ++x) {
            for (int c = 0; c < numComponentes; ++c) {
                std::swap(datosModificados[(y * ancho + x) * numComponentes + c],
                          datosModificados[(y * ancho + (ancho - 1 - x)) * numComponentes + c]);
            }
        }
    }
    
    Archivo_jpeg archivoModificado = archivo;
    archivoModificado.actualizarDatosImagen(datosModificados);
    archivoModificado.guardarImagen("_espejo_omp");
    
    auto fin = std::chrono::high_resolution_clock::now();
    double tiempo = std::chrono::duration<double, std::milli>(fin - inicio).count();
    guardarTiempoProcesamiento("espejoImagenOpenMP", tiempo);
    std::cout << "   ...Tiempo de procesamiento (espejoImagenOpenMP): " << tiempo << " ms" << std::endl;
}

void ProcesadorImagen::aplicarKernelConvolucionOpenMP(Archivo_jpeg& archivo) {
    auto inicio = std::chrono::high_resolution_clock::now();
    
    std::cout << "  > Aplicando kernel de convolución a la imagen (OpenMP): " << archivo.obtenerNombreArchivo() << std::endl;
    const auto& datosOriginales = archivo.obtenerDatosImagen();
    int ancho = archivo.obtenerAncho();
    int alto = archivo.obtenerAlto();
    int numComponentes = archivo.obtenerNumComponentes();
    
    // Kernel de ejemplo (detección de bordes)
    const int kernel[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };
    
    std::vector<unsigned char> datosModificados(datosOriginales.size());
    
    #pragma omp parallel for collapse(2)
    for (int y = 1; y < alto - 1; ++y) {
        for (int x = 1; x < ancho - 1; ++x) {
            for (int c = 0; c < numComponentes; ++c) {
                int suma = 0;
                for (int ky = -1; ky <= 1; ++ky) {
                    for (int kx = -1; kx <= 1; ++kx) {
                        int indice = ((y + ky) * ancho + (x + kx)) * numComponentes + c;
                        suma += datosOriginales[indice] * kernel[ky + 1][kx + 1];
                    }
                }
                suma = std::max(0, std::min(suma, 255));
                datosModificados[(y * ancho + x) * numComponentes + c] = static_cast<unsigned char>(suma);
            }
        }
    }
    
    Archivo_jpeg archivoModificado = archivo;
    archivoModificado.actualizarDatosImagen(datosModificados);
    archivoModificado.guardarImagen("_convolucion_omp");
    
    auto fin = std::chrono::high_resolution_clock::now();
    double tiempo = std::chrono::duration<double, std::milli>(fin - inicio).count();
    guardarTiempoProcesamiento("aplicarKernelConvolucionOpenMP", tiempo);
    std::cout << "  ...Tiempo de procesamiento (aplicarKernelConvolucionOpenMP): " << tiempo << " ms" << std::endl;
}

// =============
// = Parcial 2 =
// =============

void ProcesadorImagen::procesarImagenConDeteccionDeRostrosSinOpenMP(Archivo_jpeg& archivo) {
    auto inicio = std::chrono::high_resolution_clock::now();
    
    std::cout << "  > Procesando detección de rostros en la imagen (Sin OpenMP): " << archivo.obtenerNombreArchivo() << std::endl;

    // Obtener los datos de la imagen
    const auto& datosOriginales = archivo.obtenerDatosImagen();
    std::vector<unsigned char> datosModificados = datosOriginales;
    int ancho = archivo.obtenerAncho();
    int alto = archivo.obtenerAlto();
    int numComponentes = archivo.obtenerNumComponentes();

    // Convertir la imagen a formato OpenCV para procesamiento
    cv::Mat imagenOriginal(alto, ancho, numComponentes == 3 ? CV_8UC3 : CV_8UC1, datosModificados.data());

    // Convertir la imagen a escala de grises
    cv::Mat imagenGris;
    cv::cvtColor(imagenOriginal, imagenGris, cv::COLOR_BGR2GRAY); // Sin OpenMP

    // Cargar el clasificador de rostros (Viola-Jones Haar Cascade)
    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml")) {
        std::cerr << "Error al cargar el clasificador de rostros." << std::endl;
        return;
    }

    // Detectar rostros
    std::vector<cv::Rect> rostros;
    faceCascade.detectMultiScale(imagenGris, rostros);

    // Dibujar rectángulos alrededor de los rostros detectados
    for (const auto& rostro : rostros) {
        cv::rectangle(imagenOriginal, rostro, cv::Scalar(0, 255, 0), 2);
    }

    // Guardar la imagen modificada utilizando Archivo_jpeg
    archivo.actualizarDatosImagen(datosModificados);
    archivo.guardarImagen("_rostros_detectados");
    std::cout << "Imagen con rectángulos guardada" << std::endl;

    // Extraer y guardar los rostros individuales
    for (size_t i = 0; i < rostros.size(); ++i) {
        // Recortar el rostro de la imagen original
        cv::Mat rostroRecortado = imagenOriginal(rostros[i]);

        // Verificar si el rostro recortado tiene datos
        if (rostroRecortado.empty()) {
            std::cerr << "Rostro recortado vacío en la posición: " << i << std::endl;
            continue; // Saltar si el rostro está vacío
        }

        // Crear un nuevo archivo para cada rostro
        std::string rostroPath = "img_resultado/rostro_" + std::to_string(i) + ".jpg"; 
        Archivo_jpeg archivoRostro(rostroPath); 
        
        // Obtener el tamaño de los datos del rostro recortado
        std::vector<unsigned char> datosRostro(rostroRecortado.data, rostroRecortado.data + (rostroRecortado.total() * rostroRecortado.elemSize()));
        
        // Actualizar dimensiones
        archivoRostro.actualizarDimensiones(rostroRecortado.cols, rostroRecortado.rows, rostroRecortado.channels());
        
        // Actualizar los datos de imagen del rostro
        archivoRostro.actualizarDatosImagen(datosRostro);
        
        // Guardar el rostro usando el método directo de OpenCV
        cv::imwrite(rostroPath, rostroRecortado);
        std::cout << "Rostro guardado: " << rostroPath << std::endl; // Confirmación de guardado
    }

    // Medir y mostrar el tiempo de procesamiento
    auto fin = std::chrono::high_resolution_clock::now();
    double tiempo = std::chrono::duration<double, std::milli>(fin - inicio).count();
    guardarTiempoProcesamiento("procesarImagenConDeteccionDeRostrosSinOpenMP", tiempo);
    std::cout << "   ...Tiempo de procesamiento (procesarImagenConDeteccionDeRostrosSinOpenMP): " << tiempo << " ms" << std::endl;
}

void ProcesadorImagen::procesarImagenConDeteccionDeRostros(Archivo_jpeg& archivo) {
    auto inicio = std::chrono::high_resolution_clock::now();
    
    std::cout << "  > Procesando detección de rostros en la imagen (Con OpenMP): " << archivo.obtenerNombreArchivo() << std::endl;

    // Obtener los datos de la imagen
    const auto& datosOriginales = archivo.obtenerDatosImagen();
    std::vector<unsigned char> datosModificados = datosOriginales;
    int ancho = archivo.obtenerAncho();
    int alto = archivo.obtenerAlto();
    int numComponentes = archivo.obtenerNumComponentes();

    // Convertir la imagen a formato OpenCV para procesamiento
    cv::Mat imagenOriginal(alto, ancho, numComponentes == 3 ? CV_8UC3 : CV_8UC1, datosModificados.data());

    // Convertir la imagen a escala de grises
    cv::Mat imagenGris;
    cv::cvtColor(imagenOriginal, imagenGris, cv::COLOR_BGR2GRAY);

    // Cargar el clasificador de rostros (Viola-Jones Haar Cascade)
    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml")) {
        std::cerr << "Error al cargar el clasificador de rostros." << std::endl;
        return;
    }

    // Detectar rostros
    std::vector<cv::Rect> rostros;
    faceCascade.detectMultiScale(imagenGris, rostros);

    // Dibujar rectángulos alrededor de los rostros detectados
    for (const auto& rostro : rostros) {
        cv::rectangle(imagenOriginal, rostro, cv::Scalar(0, 255, 0), 2);
    }

    // Guardar la imagen modificada utilizando Archivo_jpeg
    archivo.actualizarDatosImagen(datosModificados);
    archivo.guardarImagen("_rostros_detectados");
    std::cout << "Imagen con rectángulos guardada" << std::endl;

    // Extraer y guardar los rostros individuales usando OpenMP
    #pragma omp parallel for
    for (size_t i = 0; i < rostros.size(); ++i) {
        // Recortar el rostro de la imagen original
        cv::Mat rostroRecortado = imagenOriginal(rostros[i]);

        // Verificar si el rostro recortado tiene datos
        if (rostroRecortado.empty()) {
            #pragma omp critical
            std::cerr << "Rostro recortado vacío en la posición: " << i << std::endl;
            continue; // Saltar si el rostro está vacío
        }

        // Crear un nuevo archivo para cada rostro
        std::string rostroPath = "img_resultado/rostro__" + std::to_string(i) + "_omp.jpg"; 
        Archivo_jpeg archivoRostro(rostroPath); 
        
        // Obtener el tamaño de los datos del rostro recortado
        std::vector<unsigned char> datosRostro(rostroRecortado.data, rostroRecortado.data + (rostroRecortado.total() * rostroRecortado.elemSize()));
        
        // Actualizar dimensiones
        archivoRostro.actualizarDimensiones(rostroRecortado.cols, rostroRecortado.rows, rostroRecortado.channels());
        
        // Actualizar los datos de imagen del rostro
        archivoRostro.actualizarDatosImagen(datosRostro);
        
        // Guardar el rostro usando el método directo de OpenCV
        if (!cv::imwrite(rostroPath, rostroRecortado)) {
            #pragma omp critical
            std::cerr << "Error al guardar el rostro: " << rostroPath << std::endl;
        } else {
            #pragma omp critical
            std::cout << "Rostro guardado: " << rostroPath << std::endl; // Confirmación de guardado
        }
    }

    // Medir y mostrar el tiempo de procesamiento
    auto fin = std::chrono::high_resolution_clock::now();
    double tiempo = std::chrono::duration<double, std::milli>(fin - inicio).count();
    guardarTiempoProcesamiento("procesarImagenConDeteccionDeRostros", tiempo);
    std::cout << "   ...Tiempo de procesamiento (procesarImagenConDeteccionDeRostros): " << tiempo << " ms" << std::endl;
}
