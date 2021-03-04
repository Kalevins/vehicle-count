/**
 * @file Blob.h
 * @author Kevin Muñoz Rengifo,
 *         Carlos Sanchez Meneses,
 *         Juan Mateo Albán Méndez,
 *         Jorge Manuel Castillo Camargo
 * @brief Cabezera blob que define la clase
 * @version 1.0
 * @date 2020-03-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef MY_BLOB
#define MY_BLOB

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

class Blob {
public:
    
    /**
     * @brief Vector con los puntos del contorno actual
     * 
     */
    std::vector<cv::Point> currentContour;
    /**
     * @brief Limite del rectangulo actual
     * 
     */
    cv::Rect currentBoundingRect;
    /**
     * @brief Vector con los puntos de la posición central
     * 
     */
    std::vector<cv::Point> centerPositions;
    /**
     * @brief Tamaño diagonal actual
     * 
     */
    double dblCurrentDiagonalSize;
    /**
     * @brief Relación de aspecto actual
     * 
     */
    double dblCurrentAspectRatio;
    /**
     * @brief Coincidencia actual encontrada o nuevo Blob
     * 
     */
    bool blnCurrentMatchFoundOrNewBlob;
    /**
     * @brief Rastreo actual
     * 
     */
    bool blnStillBeingTracked;
    /**
     * @brief Número de fotogramas consecutivos sin una coincidencia
     * 
     */
    int intNumOfConsecutiveFramesWithoutAMatch;
    /**
     * @brief Siguiente posición prevista
     * 
     */
    cv::Point predictedNextPosition;
    /**
     * @brief Constructor de un nuevo obejeto Blob
     * 
     * @param _contour Contorno
     */
    Blob(std::vector<cv::Point> _contour);
    /**
     * @brief Predice la siguiente posición del vehiculo
     * 
     */
    void predictNextPosition(void);

};

#endif