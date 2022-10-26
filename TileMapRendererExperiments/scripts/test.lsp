
(def grass-centers [964 889 890 1019])
(def grass-center-weights [0.33 0.33 0.33 0.01])
(def house-max-w 20)
(def house-min-w 4)

(def house-max-h 20)
(def house-min-h 4)

(def roof-max-h 20)
(def roof-min-h 4)

(def house-bottom-left-edge 445)
(def house-bottom-right-edge 446)
(def house-bottom-middle 299)

(def house-middle-left-edge 408)
(def house-middle-right-edge 409)

(def house-roof-back-left-corner 1)
(def house-roof-back-right-corner 2)
(def house-roof-back-middle 3)
(def house-roof-left-edge 4)
(def house-roof-right-edge 41)
(def house-roof-front-left-corner 38)
(def house-roof-front-right-corner 39)
(def house-roof-front-middle 40)
(def house-roof-middle 7)

(def doors [983 984 986 989 1020 1021 1023 1024 1026 1027])

(def house-middle 188)

(var houses @[])

(defn get-tile-of-house-body
    "get a tile for the house body based on x and y of the loop"
    [loop-x loop-y loop-x-max loop-y-max]
    (cond
        (and (= loop-x 0) (= loop-y 0))                house-bottom-left-edge
        (and (= loop-x (- loop-x-max 1)) (= loop-y 0)) house-bottom-right-edge
        (= loop-x 0)                                   house-middle-left-edge
        (= loop-x (- loop-x-max 1))                    house-middle-right-edge
        (= loop-y 0)                                   house-bottom-middle
                                                       house-middle))

(defn get-tile-of-house-roof
    "get a tile for the houses roof based on x and y of the loop"
    [loop-x loop-y loop-x-max loop-y-max]
    (cond
        (and (= loop-x 0) (= loop-y 0))                               house-roof-front-left-corner
        (and (= loop-x (- loop-x-max 1)) (= loop-y 0))                house-roof-front-right-corner
        (and (= loop-x 0) (= loop-y (- loop-y-max 1)))                house-roof-back-left-corner
        (and (= loop-x (- loop-x-max 1)) (= loop-y (- loop-y-max 1))) house-roof-back-right-corner
        (= loop-x 0)                                                  house-roof-left-edge
        (= loop-x (- loop-x-max 1))                                   house-roof-right-edge
        (= loop-y 0)                                                  house-roof-front-middle
        (= loop-y (- loop-y-max 1))                                   house-roof-back-middle
                                                                      house-roof-middle))

(defn generate-random-house-body
    "generates a random house base"
    [bottom-left-x bottom-left-y]
    (def random-w (random-u32-between house-min-w house-max-w))
    (def random-h (random-u32-between house-min-h house-max-h))
    (loop [x :range [0 random-w]]
        (loop [y :range [0 random-h]]
            (set-tile-at (+ bottom-left-x x) (- bottom-left-y y) (get-tile-of-house-body x y random-w random-h))))
    { 
        :bottom-left { :x bottom-left-x :y bottom-left-y}
        :dimensions { :x random-w :y random-h}
    })

(defn generate-random-house-roof
    "adds a roof to a house base"
    [house-base]
    (def random-h (random-u32-between roof-min-h roof-max-h))
    (def house-base-w (get (get house-base :dimensions) :x))
    (def house-base-h (get (get house-base :dimensions) :y))
    (def house-base-bottom-left-y (get (get house-base :bottom-left) :y))
    (def house-base-bottom-left-y (get (get house-base :bottom-left) :x))

    (def bottom-left-y (- house-base-bottom-left-y house-base-h))
    (def bottom-left-x (get (get house-base :bottom-left) :x))
    (loop [x :range [0 house-base-w]]
        (loop [y :range [0 random-h]]
            (set-tile-at (+ bottom-left-x x) (- bottom-left-y y) (get-tile-of-house-roof x y house-base-w random-h))))
    {
        :bottom-left { :x bottom-left-x :y house-base-bottom-left-y}
        :dimensions { :x house-base-w :y house-base-h}
        :roof-dimensions { :x house-base-w :y random-h}
    })

(defn generate-random-house-doors
    "adds doors to a single house"
    [house]
    (def house-bottom-left (get house :bottom-left))
    (def house-w (get (get house :dimensions):x))
    (def door-x (random-u32-between 0 (- house-w 1)))
    (def door-tile (get doors (random-u32-between 0 (-(length doors)1))))
    (set-tile-at ( + (get house-bottom-left :x) door-x) (get house-bottom-left :y) door-tile))

(defn add-roofs-to-houses
    "adds roofs to a list of houses"
    [houses]
    (map (fn[house] (generate-random-house-roof house)) houses))

(defn add-doors-to-houses
    "adds doors to a list of houses"
    [houses]
    (map (fn[house] (generate-random-house-doors house)) houses))

(defn generate-map
    "called by game when the tiled world needs to be generated"
    [layer layer-w layer-h]
    (case layer
        0 (flood-layer-weighted grass-centers grass-center-weights)
        1 (array/push houses (generate-random-house-body 100 100))
        2 (set houses (add-roofs-to-houses houses))
        3 (add-doors-to-houses houses)
        4 (add-roofs-to-houses houses)))
