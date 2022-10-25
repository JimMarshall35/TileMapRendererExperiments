
(def grass-centers [964 889 890 1019])
(def grass-center-weights [0.33 0.33 0.33 0.01])
(def house-max-w 20)
(def house-min-w 4)

(def house-max-h 20)
(def house-min-h 4)

(def house-bottom-left-edge 445)
(def house-bottom-right-edge 446)
(def house-bottom-middle 299)

(def house-middle-left-edge 408)
(def house-middle-right-edge 409)

(def house-middle 188)

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

(defn generate-random-house-body
    "generates a random house"
    [bottom-left-x bottom-left-y]
    (def random-w (random-u32-between house-min-w house-max-w))
    (def random-h (random-u32-between house-min-h house-max-h))
    (loop [x :range [0 random-w]]
        (loop [y :range [0 random-h]]
            (set-tile-at (+ bottom-left-x x) (- bottom-left-y y) (get-tile-of-house-body x y random-w random-h))))
    { 
        :bottom-left { :x bottom-left-x :y bottom-left-x}
        :dimensions { :x random-w :y random-h}
    })

(defn generate-map
    "testing"
    [layer layer-w layer-h]
    (if (= layer 0)
        (flood-layer-weighted grass-centers grass-center-weights))
    (if (= layer 1)
        (generate-random-house-body 100 100)))