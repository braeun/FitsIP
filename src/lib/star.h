#ifndef STAR_H
#define STAR_H

/**
 * @brief This class contains all data of a star found by the star finding algorithm.
 */
class Star {
  public:
    double x;         /* x position of center */
    double y;         /* y position of center */
    double fwhm;      /* full width half maximum */
    double xwidth;    /* fwhm width in x direction */
    double ywidth;    /* fwhm width in y direction */
    double round;     /* roundness criterion */
    double sharp;     /* sharpness criterion */
    double hotness;   /* hotness criterion */
};

#endif // STAR_H
